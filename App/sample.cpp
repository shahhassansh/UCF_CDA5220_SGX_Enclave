#include <string.h>
#include <assert.h>

#include <unistd.h>
#include <pwd.h>
#include <libgen.h>
#include <stdlib.h>

# define MAX_PATH FILENAME_MAX


#include <sgx_urts.h>
#include "../App/sample.h"

#include "Enclave1_u.h"
#include "Enclave2_u.h"



/* Global EID shared by multiple threads */
sgx_enclave_id_t enclave1_eid = 0;
sgx_enclave_id_t enclave2_eid = 0;

typedef struct _sgx_errlist_t {
    sgx_status_t err;
    const char *msg;
    const char *sug; /* Suggestion */
} sgx_errlist_t;

/* Error code returned by sgx_create_enclave */
static sgx_errlist_t sgx_errlist[] = {
    {
        SGX_ERROR_UNEXPECTED,
        "Unexpected error occurred.",
        NULL
    },
    {
        SGX_ERROR_INVALID_PARAMETER,
        "Invalid parameter.",
        NULL
    },
    {
        SGX_ERROR_OUT_OF_MEMORY,
        "Out of memory.",
        NULL
    },
    {
        SGX_ERROR_ENCLAVE_LOST,
        "Power transition occurred.",
        "Please refer to the sample \"PowerTransition\" for details."
    },
    {
        SGX_ERROR_INVALID_ENCLAVE,
        "Invalid enclave image.",
        NULL
    },
    {
        SGX_ERROR_INVALID_ENCLAVE_ID,
        "Invalid enclave identification.",
        NULL
    },
    {
        SGX_ERROR_INVALID_SIGNATURE,
        "Invalid enclave signature.",
        NULL
    },
    {
        SGX_ERROR_OUT_OF_EPC,
        "Out of EPC memory.",
        NULL
    },
    {
        SGX_ERROR_NO_DEVICE,
        "Invalid Intel(R) SGX device.",
        "Please make sure Intel(R) SGX module is enabled in the BIOS, and install Intel(R) SGX driver afterwards."
    },
    {
        SGX_ERROR_MEMORY_MAP_CONFLICT,
        "Memory map conflicted.",
        NULL
    },
    {
        SGX_ERROR_INVALID_METADATA,
        "Invalid enclave metadata.",
        NULL
    },
    {
        SGX_ERROR_DEVICE_BUSY,
        "Intel(R) SGX device was busy.",
        NULL
    },
    {
        SGX_ERROR_INVALID_VERSION,
        "Enclave version was invalid.",
        NULL
    },
    {
        SGX_ERROR_INVALID_ATTRIBUTE,
        "Enclave was not authorized.",
        NULL
    },
    {
        SGX_ERROR_ENCLAVE_FILE_ACCESS,
        "Can't open enclave file.",
        NULL
    },
};

/* Check error conditions for loading enclave */
void print_error_message(sgx_status_t ret)
{
    size_t idx = 0;
    size_t ttl = sizeof sgx_errlist/sizeof sgx_errlist[0];

    for (idx = 0; idx < ttl; idx++) {
        if(ret == sgx_errlist[idx].err) {
            if(NULL != sgx_errlist[idx].sug)
                printf("Info: %s\n", sgx_errlist[idx].sug);
            printf("Error: %s\n", sgx_errlist[idx].msg);
            break;
        }
    }
    
    if (idx == ttl)
        printf("Error code is 0x%X. Please refer to the \"Intel SGX SDK Developer Reference\" for more details.\n", ret);
}

/* Initialize the enclave:
 *   Call sgx_create_enclave to initialize an enclave instance
 */
int initialize_enclave(const char * filename, sgx_enclave_id_t* id ){

                   sgx_status_t ret = SGX_ERROR_UNEXPECTED;

                  ret = sgx_create_enclave(filename, SGX_DEBUG_FLAG, NULL, NULL, id, NULL);

                   if (ret != SGX_SUCCESS)

                  {

                                    printf("Error code 0x%X.",ret);

                                    return -1;

                  }

                  return 0;

}

/* OCall functions */
void ocall_Main_sample(const char *str)
{
    /* Proxy/Bridge will check the length and null-terminate 
     * the input string to prevent buffer overflow. 
     */
    printf("%s", str);
}


/* Application entry */
int SGX_CDECL main(int argc, char *argv[]){

                  //Working With Enclave 1

                 if(initialize_enclave(Enclave1_FILENAME, &enclave1_eid) < 0)

                                   return -1;

                  sgx_status_t ret = SGX_ERROR_UNEXPECTED;

                 int ecall_return = 0;

                  ret = Enclave1_ecall_Main_sample(enclave1_eid, &ecall_return);

                  if (ret != SGX_SUCCESS)

                                   abort();

                  sgx_destroy_enclave(enclave1_eid);



                 //Working With Enclave 2

                 if(initialize_enclave(Enclave2_FILENAME, &enclave2_eid) < 0)

                                   return -1;

                 ret = SGX_ERROR_UNEXPECTED;

                 ret = Enclave2_ecall_Main_sample(enclave2_eid, &ecall_return);

                 if (ret != SGX_SUCCESS)

                                   abort();

                 sgx_destroy_enclave(enclave2_eid);

                 return 0;

}
