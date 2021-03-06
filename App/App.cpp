/*
 * Copyright (C) 2011-2019 Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


// App.cpp : Defines the entry point for the console application.
#include <stdio.h>
#include <map>
#include <time.h>
#include "../Enclave1/Enclave1_u.h"
#include "../Enclave2/Enclave2_u.h"
#include "sgx_eid.h"
#include "sgx_urts.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>


#define UNUSED(val) (void)(val)
#define TCHAR   char
#define _TCHAR  char
#define _T(str) str
#define scanf_s scanf
#define _tmain  main

extern std::map<sgx_enclave_id_t, uint32_t>g_enclave_id_map;


sgx_enclave_id_t e1_enclave_id = 0;
sgx_enclave_id_t e2_enclave_id = 0;

#define ENCLAVE1_PATH "libenclave1.so"
#define ENCLAVE2_PATH "libenclave2.so"

void ocall_print(const char* str) {
    printf("%s\n", str);
}

void ocall_print_int(int str) {
    printf("%d\n", str);
}

void convert_int_to_string(int convert_int, char* return_string) {
    sprintf(return_string, "%d", convert_int);
}

int get_current_time() {
    time_t now = time(0);
    return now;
}

uint32_t load_enclaves()
{
    uint32_t enclave_temp_no;
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;

    enclave_temp_no = 0;

    ret = sgx_create_enclave(ENCLAVE1_PATH, SGX_DEBUG_FLAG, NULL, NULL, &e1_enclave_id, NULL);
    if (ret != SGX_SUCCESS) {
                return ret;
    }

    enclave_temp_no++;
    g_enclave_id_map.insert(std::pair<sgx_enclave_id_t, uint32_t>(e1_enclave_id, enclave_temp_no));

    ret = sgx_create_enclave(ENCLAVE2_PATH, SGX_DEBUG_FLAG, NULL, NULL, &e2_enclave_id, NULL);
    if (ret != SGX_SUCCESS) {
                return ret;
    }

    enclave_temp_no++;
    g_enclave_id_map.insert(std::pair<sgx_enclave_id_t, uint32_t>(e2_enclave_id, enclave_temp_no));


    return SGX_SUCCESS;
}

int _tmain(int argc, _TCHAR* argv[])
{
    uint32_t ret_status;
    long ret_status_long;
    sgx_status_t status;

    UNUSED(argc);
    UNUSED(argv);

    if(load_enclaves() != SGX_SUCCESS)
    {
        printf("\nLoad Enclave Failure");
    }

    printf("\nAvailable Enclaves");
    printf("\nEnclave1 - EnclaveID %" PRIx64, e1_enclave_id);
    printf("\nEnclave2 - EnclaveID %" PRIx64, e2_enclave_id);
    printf("\n");


    printf("Welcome to the OTP Server\n");
    printf("Type \"help\"\n");


    while (true)
    {

        // std::string command;
        char strCommand[100];

        char str1[100];
        char str2[100];
        char str3[100];
        char str4[100];
        int i;
        void *encrypt = malloc(100);

        printf("Enter a command: ");

        // std::getline (std::cin, command);

        scanf("%[^\n]%*c", strCommand);
        char* split = strtok(strCommand, " ");
        strcpy(str1, split);

        if (strcmp(str1, "help") == 0) {
            printf("Example Usage\n");
            printf("create-account\n");
            printf("get-OTP\n");
            printf("quit\n");

        } 
        else if (strcmp(str1, "quit") == 0) 
        {
            break;
        } else if (strcmp(str1, "create-account") == 0) {

            char* OTP_SECRET = (char*) malloc(10);
            status = Enclave1_generate_otp_secret(e1_enclave_id, &ret_status, OTP_SECRET, 10);
            if (status!=SGX_SUCCESS)
            {
                printf("Enclave1_test_create_session Ecall failed: Error code is %x", status);
                break;
            }

            int get_password_return;

            status = Enclave1_test_create_session(e1_enclave_id, &ret_status, e1_enclave_id, e2_enclave_id);
            if (status!=SGX_SUCCESS)
            {
                printf("Enclave1_test_create_session Ecall failed: Error code is %x", status);
                break;
            }
            else
            {
                if(ret_status==0)
                {
                    printf("\nSecure Channel Establishment between Source (E1) and Destination (E2) Enclaves successful !!!");
                }
                else
                {
                    printf("\nSession establishment and key exchange failure between Source (E1) and Destination (E2): Error code is %x", ret_status);
                    break;
                }
            }

            // TODO: We can only exchange messages of max len 8 

            //Test message exchange between Enclave1(Source) and Enclave2(Destination)
            status = Enclave1_test_message_exchange(e1_enclave_id, &ret_status, e1_enclave_id, e2_enclave_id, OTP_SECRET, strlen(OTP_SECRET));
            if (status!=SGX_SUCCESS)
            {
                printf("Enclave1_test_message_exchange Ecall failed: Error code is %x", status);
                break;
            }
            else
            {
                if(ret_status==0)
                {
                    printf("\nMessage Exchange between Source (E1) and Destination (E2) Enclaves successful !!!");
                }
                else
                {
                    printf("\nMessage Exchange failure between Source (E1) and Destination (E2): Error code is %x", ret_status);
                    break;
                }
            }

            
        

            //Test Closing Session between Enclave1(Source) and Enclave2(Destination)
            status = Enclave1_test_close_session(e1_enclave_id, &ret_status, e1_enclave_id, e2_enclave_id);
            if (status!=SGX_SUCCESS)
            {
                printf("Enclave1_test_close_session Ecall failed: Error code is %x", status);
                break;
            }
            else
            {
                if(ret_status==0)
                {
                    printf("\nClose Session between Source (E1) and Destination (E2) Enclaves successful !!!");
                }
                else
                {
                    printf("\nClose session failure between Source (E1) and Destination (E2): Error code is %x", ret_status);
                    break;
                }
            }
            printf("\n");

        } else if (strcmp(str1, "get-OTP") == 0) {

            char* otp_code = (char*) malloc(10);

            status = Enclave2_generate_OTP_from_secret(e2_enclave_id, &ret_status_long, otp_code, 10);
            if (status!=SGX_SUCCESS)
            {
                printf("Enclave2_test_create_session Ecall failed: Error code is %x", status);
                break;
            }
            printf("%ld", ret_status_long);
            if (ret_status == 1) {
                printf("Enclave2 Generated OTP Code: ");
                printf(otp_code);
                printf("\n");
            } else {
                printf("Enclave2: OTP Secret not initialized correctly\n");
            }
            
            /*
            printf("\n");
                printf("\n");
                printf("ENCLAVE2 Sending Pong---------\n");
                //Test Create session between Enclave2(Source) and Enclave1(Destination)
                status = Enclave2_test_create_session(e2_enclave_id, &ret_status, e2_enclave_id, e1_enclave_id);
                if (status!=SGX_SUCCESS)
                {
                    printf("Enclave2_test_create_session Ecall failed: Error code is %x", status);
                    break;
                }
                else
                {
                    if(ret_status==0)
                    {
                        printf("\n\nSecure Channel Establishment between Source (E2) and Destination (E1) Enclaves successful !!!");
                    }
                    else
                    {
                        printf("\nSession establishment and key exchange failure between Source (E2) and Destination (E1): Error code is %x", ret_status);
                        break;
                    }
                }



                status = Enclave2_test_message_exchange(e2_enclave_id, &ret_status, e2_enclave_id, e1_enclave_id, "pong", 4);
                if (status!=SGX_SUCCESS)
                {
                    printf("Enclave2_test_message_exchange Ecall failed: Error code is %x", status);
                    break;
                }
                else
                {
                    if(ret_status==0)
                    {
                        printf("\n\nMessage Exchange between Source (E2) and Destination (E1) Enclaves successful !!!");
                    }
                    else
                    {
                        printf("\n\nMessage Exchange failure between Source (E2) and Destination (E1): Error code is %x", ret_status);
                        break;
                    }
                }

                //Test Closing Session between Enclave1(Source) and Enclave2(Destination)
                status = Enclave2_test_close_session(e2_enclave_id, &ret_status, e2_enclave_id, e1_enclave_id);
                if (status!=SGX_SUCCESS)
                {
                    printf("Enclave2_test_close_session Ecall failed: Error code is %x", status);
                    break;
                }
                else
                {
                    if(ret_status==0)
                    {
                        printf("\n\nClose Session between Source (E2) and Destination (E1) Enclaves successful !!!");
                    }
                    else
                    {
                        printf("\n\nClose session failure between Source (E2) and Destination (E1): Error code is %x", ret_status);
                        break;
                    }
                }
                printf("\n");
                */


        } else {
            break;
        }
    }


    sgx_destroy_enclave(e1_enclave_id);
    sgx_destroy_enclave(e2_enclave_id);

    return 0;
}
