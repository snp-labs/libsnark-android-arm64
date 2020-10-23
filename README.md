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


-   __ANDROID_TOOLCHAIN=4.7__ : Select compiler version
-   ___ANDROID_STL=c++_shared___ : If you link library, you need this option
-   __WITH_PROCPS=OFF__ : libprocps is not necessary to link the library. 
-   __CURVE=ALT_BN128__ : use ALT_BN128
-   __WITH_SUPERCOP=OFF__ : supercop is assembly language.(can't use at arm machine)
-   __OPT_FLAGS=-Os -march=armv8-a__ : select machine
-   __PERFORMANCE=ON__ : OFF DEBUG MODE

--------------------------------------------------------------------------------
Project Structure
--------------------------------------------------------------------------------
-   __MINIMUM SDK VERSION__ : __23__(ANDROID 6.0 Marshmallow)
-   __NDK VERSION__ : 21.3.6528147
-   __Android Gradle__ Plugin Version : 4.0.1
-   __Gradle Version__ : 6.1.1
