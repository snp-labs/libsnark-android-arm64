using namespace libsnark;

template<typename ppT>
bool test_r1cs_gg_ppzksnark(size_t num_constraints,
                         size_t input_size)
{
    libff::print_header("(enter) Test R1CS GG-ppzkSNARK");

    const bool test_serialization = true;
    r1cs_example<libff::Fr<ppT> > example = generate_r1cs_example_with_binary_input<libff::Fr<ppT> >(num_constraints, input_size);
    const bool bit = run_r1cs_gg_ppzksnark<ppT>(example, test_serialization);
    assert(bit);

    libff::print_header("(leave) Test R1CS GG-ppzkSNARK");
    return bit;
}

JNIEXPORT bool JNICALL Java_HelloWorldJNI_sayHello
  (JNIEnv* env, jobject thisObject) {
    default_r1cs_gg_ppzksnark_pp::init_public_params();
    libff::start_profiling();
    return test_r1cs_gg_ppzksnark<default_r1cs_gg_ppzksnark_pp>(1000, 100);
}
