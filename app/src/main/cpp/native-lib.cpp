#define NO_PROCPS
#define CURVE_ALT_BN128
//#define MIE_ATE_USE_GMP
#define NDEBUG
//#define _FILE_OFFSET_BITS 64
//#define MIE_ZM_VUINT_BIT_LEN (64 * 9)
//#define NDEBUG 1
#include <android/log.h>
#define  LOG_TAG    "NDK_TEST"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

#include <jni.h>
#include <string>
#include <iostream>
#include <gmp.h>
#include <gmpxx.h>
#include <openssl/bn.h>
#include <cassert>
#include <cstdio>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <type_traits>
#include <libff/common/profiling.hpp>
#include <libff/common/utils.hpp>
#include <libff/algebra/curves/public_params.hpp>
#include <libsnark/common/default_types/r1cs_gg_ppzksnark_pp.hpp>
#include <libsnark/relations/constraint_satisfaction_problems/r1cs/examples/r1cs_examples.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_gg_ppzksnark/examples/run_r1cs_gg_ppzksnark.hpp>
using namespace libsnark;
using namespace std;

template<typename ppT>
bool run1_r1cs_gg_ppzksnark(const r1cs_example<libff::Fr<ppT> > &example,
                           const bool test_serialization)
{
    libff::enter_block("Call to run_r1cs_gg_ppzksnark");

    LOGD("R1CS GG-ppzkSNARK Generator");
    r1cs_gg_ppzksnark_keypair<ppT> keypair = r1cs_gg_ppzksnark_generator<ppT>(example.constraint_system);
    printf("\n"); libff::print_indent(); libff::print_mem("after generator");
    r1cs_gg_ppzksnark_processed_verification_key<ppT> pvk = r1cs_gg_ppzksnark_verifier_process_vk<ppT>(keypair.vk);

//    if (test_serialization)
//    {
//        libff::enter_block("Test serialization of keys");
//        keypair.pk = libff::reserialize<r1cs_gg_ppzksnark_proving_key<ppT> >(keypair.pk);
//        keypair.vk = libff::reserialize<r1cs_gg_ppzksnark_verification_key<ppT> >(keypair.vk);
//        pvk = libff::reserialize<r1cs_gg_ppzksnark_processed_verification_key<ppT> >(pvk);
//        libff::leave_block("Test serialization of keys");
//    }




    LOGD("R1CS GG-ppzkSNARK Prover");
    r1cs_gg_ppzksnark_proof<ppT> proof = r1cs_gg_ppzksnark_prover<ppT>(keypair.pk, example.primary_input, example.auxiliary_input);
    printf("\n"); libff::print_indent(); libff::print_mem("after prover");

//    if (test_serialization)
//    {
//        libff::enter_block("Test serialization of proof");
//        proof = libff::reserialize<r1cs_gg_ppzksnark_proof<ppT> >(proof);
//        libff::leave_block("Test serialization of proof");
//    }
//
    LOGD("R1CS GG-ppzkSNARK Verifier");

    //proof.g_A = libff::G1<ppT>::random_element();
    const bool ans = r1cs_gg_ppzksnark_verifier_strong_IC<ppT>(keypair.vk, example.primary_input, proof);
    printf("\n"); libff::print_indent(); libff::print_mem("after verifier");
    LOGD("* The verification result is: %s\n", (ans ? "PASS" : "FAIL"));

    LOGD("R1CS GG-ppzkSNARK Online Verifier");
    const bool ans2 = r1cs_gg_ppzksnark_online_verifier_strong_IC<ppT>(pvk, example.primary_input, proof);
    assert(ans == ans2);

    test_affine_verifier<ppT>(keypair.vk, example.primary_input, proof, ans);

    libff::leave_block("Call to run_r1cs_gg_ppzksnark");



    const r1cs_gg_ppzksnark_verification_key<ppT> &vk = keypair.vk;
    r1cs_gg_ppzksnark_processed_verification_key<ppT> pvk_ = r1cs_gg_ppzksnark_verifier_process_vk<ppT>(vk);
    LOGD("accumulation_vector");
    //const accumulation_vector<libff::G1<ppT> > accumulated_IC = pvk.gamma_ABC_g1.template accumulate_chunk<libff::Fr<ppT> >(primary_input.begin(), primary_input.end(), 0);
    libff::G1<ppT> acc1 = libff::G1<ppT>::zero();
    LOGD("example.primary_input.size() = %d",example.primary_input.size());
    LOGD("pvk.gamma_ABC_g1.rest.size() = %d",pvk_.gamma_ABC_g1.rest.size());
    for(size_t i = 0; i< pvk_.gamma_ABC_g1.rest.size(); i++)
        acc1 = example.primary_input[i]*pvk_.gamma_ABC_g1.rest[i] + acc1;
    acc1 = pvk_.gamma_ABC_g1.first + acc1;
    libff::G1<ppT> &acc = acc1;
    bool result = true;
    if (!proof.is_well_formed())
    {
        if (!libff::inhibit_profiling_info)
        {
            libff::print_indent(); printf("At least one of the proof elements does not lie on the curve.\n");
        }
        result = false;
    }
    const libff::G1_precomp<ppT> proof_g_A_precomp = ppT::precompute_G1(proof.g_A);
    const libff::G2_precomp<ppT> proof_g_B_precomp = ppT::precompute_G2(proof.g_B);
    const libff::G1_precomp<ppT> proof_g_C_precomp = ppT::precompute_G1(proof.g_C);
    const libff::G1_precomp<ppT> acc_precomp = ppT::precompute_G1(acc1);

    const libff::Fqk<ppT> QAP1 = ppT::miller_loop(proof_g_A_precomp,  proof_g_B_precomp);
    const libff::Fqk<ppT> QAP2 = ppT::double_miller_loop(
            acc_precomp, pvk_.vk_gamma_g2_precomp,
            proof_g_C_precomp, pvk_.vk_delta_g2_precomp);
    const libff::GT<ppT> QAP = ppT::final_exponentiation(QAP1 * QAP2.unitary_inverse());
    if (QAP != pvk_.vk_alpha_g1_beta_g2)
    {
        LOGD("QAP verify = %d", QAP == pvk_.vk_alpha_g1_beta_g2);
        if (!libff::inhibit_profiling_info)
        {
            LOGD("QAP divisibility check failed.");
            libff::print_indent(); printf("QAP divisibility check failed.\n");
        }
        result = false;
    }
    LOGD("result = %d", result);



    return ans;
}

template<typename ppT>
bool test_r1cs_gg_ppzksnark(size_t num_constraints,
                            size_t input_size)
{
    LOGD("(enter) Test R1CS GG-ppzkSNARK");

    const bool test_serialization = true;
    r1cs_example<libff::Fr<ppT> > example = generate_r1cs_example_with_binary_input<libff::Fr<ppT> >(num_constraints, input_size);
    const bool bit = run1_r1cs_gg_ppzksnark<ppT>(example, test_serialization);
    return bit;

    //libff::print_header("(leave) Test R1CS GG-ppzkSNARK");
}
extern "C" JNIEXPORT jstring JNICALL
Java_com_example_snarkportingtest_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    mpz_t bn_a, bn_b, bn_sum;
    const char *cc_a, *cc_b;
    jstring jst_sum;
    char *c_sum;
    BIGNUM *key = BN_new();

    int a = sizeof(unsigned long int);
    mpz_inits(bn_a, bn_b, bn_sum, NULL);

    std::cout << "Hello from C++ !!" << std::endl;
    libff::alt_bn128_pp::init_public_params();

    libff::start_profiling();
    bool bit = test_r1cs_gg_ppzksnark<default_r1cs_gg_ppzksnark_pp>(1000, 100);
    int bits = bit;
    string ssInt= to_string(bits);
    BN_set_word(key,12);
    return env->NewStringUTF(ssInt.c_str());
}