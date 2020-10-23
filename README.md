# libsnark-porting
This android-studio project only ported libsnark to __arm64__.
armeabi-v7a(arm32) have some error at prove and verify.
--------------------------------------------------------------------------------
Elliptic curve choices
--------------------------------------------------------------------------------
use __ALT_BN128__. more detail https://github.com/scipr-lab/libsnark

--------------------------------------------------------------------------------
CMake option
--------------------------------------------------------------------------------
vi ~/your-workspace/snarkportingtest/app/build.gradle


__-DANDROID_TOOLCHAIN=4.7__ : Select compiler version
__-DANDROID_STL=c++_shared__ : If you link library, you need this option
__-DWITH_PROCPS=OFF__ : libprocps is not necessary to link the library. 
__-DCURVE=ALT_BN128__ : use ALT_BN128
__-DWITH_SUPERCOP=OFF__ : supercop is assembly language.(can't use at arm machine)
__-DOPT_FLAGS=-Os -march=armv8-a__ : select machine
__-DPERFORMANCE=ON__ : OFF DEBUG MODE

--------------------------------------------------------------------------------
Project Structure
--------------------------------------------------------------------------------
MINIMUM SDK VERSION : __23__(ANDROID 6.0 Marshmallow)
NDK VERSION : 21.3.6528147
Android Gradle Plugin Version : 4.0.1
Gradle Version : 6.1.1
