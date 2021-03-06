Original Documentation at : https://github.com/ShivKushwah/password-manager
OTP-Server Implementation
Implements OTP server in https://people.eecs.berkeley.edu/~sseshia/pubdir/ccs15-moat.pdf
You can "create-account" which generates the OTP secret key and transfers it from the Bank enclave to the client enclave and "get-OTP" which generates the OTP code from the client enclave by hashing the current time and the OTP secret key.

--Example Usage--
source /home/shiv/Research/Intel-SGX-Installation/linux-sgx/linux/installer/bin/sgxsdk/environment
make SGX_MODE=SIM
./app
Video Demo at https://youtu.be/dpkI3GsSuGU

---Sample Code Description Below---
---------------------------
Purpose of LocalAttestation
---------------------------
The project demonstrates:
- How to establish a protected channel
- Secret message exchange using enclave to enclave function calls

------------------------------------
How to Build/Execute the Sample Code
------------------------------------
1. Install Intel(R) Software Guard Extensions (Intel(R) SGX) SDK for Linux* OS
2. Make sure your environment is set:
    $ source ${sgx-sdk-install-path}/environment
3. Build the project with the prepared Makefile:
    a. Hardware Mode, Debug build:
        $ make
    b. Hardware Mode, Pre-release build:
        $ make SGX_PRERELEASE=1 SGX_DEBUG=0
    c. Hardware Mode, Release build:
        $ make SGX_DEBUG=0
    d. Simulation Mode, Debug build:
        $ make SGX_MODE=SIM
    e. Simulation Mode, Pre-release build:
        $ make SGX_MODE=SIM SGX_PRERELEASE=1 SGX_DEBUG=0
    f. Simulation Mode, Release build:
        $ make SGX_MODE=SIM SGX_DEBUG=0
    g. Use Local Attestation 2.0 protocol, Hardware Mode, Debug build:
        $ make LAv2=1
       Note: Local Attestaion 2.0 protocol will be used if 'LAv2' is defined.
4. Execute the binary directly:
    $ ./app
5. Remember to "make clean" before switching build mode

-------------------------------------------------
Launch token initialization
-------------------------------------------------
If using libsgx-enclave-common or sgxpsw under version 2.4, an initialized variable launch_token needs to be passed as the 3rd parameter of API sgx_create_enclave. For example,

sgx_launch_token_t launch_token = {0};
sgx_create_enclave(ENCLAVE_FILENAME, SGX_DEBUG_FLAG, launch_token, NULL, &global_eid, NULL);
