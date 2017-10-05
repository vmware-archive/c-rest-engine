/* C-REST-Engine
*
* Copyright (c) 2017 VMware, Inc. All Rights Reserved. 
*
* This product is licensed to you under the Apache 2.0 license (the "License").
* You may not use this product except in compliance with the Apache 2.0 License.  
*
* This product may include a number of subcomponents with separate copyright 
* notices and license terms. Your use of these subcomponents is subject to the 
* terms and conditions of the subcomponent's license, as noted in the LICENSE file. 
*
*/

#include "includes.h"

pthread_mutex_t*                     gTESTSSLThreadLock = NULL;
pthread_mutex_t                      gTESTGlobalMutex = PTHREAD_MUTEX_INITIALIZER;
int                                  gTESTSSLisedInstaceCount = -1;


static
void
VmTESTSSLThreadLockCallback(
    int                              mode,
    int                              type,
    char*                            file,
    int                              line
    );

static
unsigned long
VmTESTSSLThreadId(
    void
    );

static
uint32_t
VmTESTSSLThreadLockInit(
    void
    );

static
void
VmTESTSSLThreadLockShutdown(
    void
    );

static
uint32_t
VmTESTSecureSocket(
    char*                            certificate,
    char*                            key,
    SSL_CTX**                        ppSSLCtx
    );


static
void
VmTESTSSLThreadLockCallback(
    int                              mode,
    int                              type,
    char*                            file,
    int                              line
    )
{
    (void)line;
    (void)file;
    if(mode & CRYPTO_LOCK)
    {
        pthread_mutex_lock(&(gTESTSSLThreadLock[type]));
    }
    else
    {
        pthread_mutex_unlock(&(gTESTSSLThreadLock[type]));
    }
}

static
unsigned long
VmTESTSSLThreadId(
    void
    )
{
    unsigned long                      ret = 0;

    ret = (unsigned long)pthread_self();
    return ret;
}

static
uint32_t
VmTESTSSLThreadLockInit(
    void
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    int                              i = 0;

    gTESTSSLThreadLock = (pthread_mutex_t *)OPENSSL_malloc(
                                           CRYPTO_num_locks() * sizeof(pthread_mutex_t)
                                           );
    if (gTESTSSLThreadLock == NULL)
    {
        dwError = 102;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    for(i = 0; i < CRYPTO_num_locks(); i++) 
    {
        pthread_mutex_init(&(gTESTSSLThreadLock[i]), NULL);
    }

    CRYPTO_set_id_callback((unsigned long (*)())VmTESTSSLThreadId);
    CRYPTO_set_locking_callback((void (*)())VmTESTSSLThreadLockCallback);

cleanup:
    return dwError;
error:
    dwError = VMREST_TRANSPORT_SSL_ERROR;
    goto cleanup;
}

static
void
VmTESTSSLThreadLockShutdown(
    void
    )
{
    int                              i = 0;

    CRYPTO_set_locking_callback(NULL);

    for( i = 0; i < CRYPTO_num_locks(); i++)
    {
        pthread_mutex_destroy(&(gTESTSSLThreadLock[i]));
    }
    OPENSSL_free(gTESTSSLThreadLock);
}

static
uint32_t
VmTESTSecureSocket(
    char*                            certificate,
    char*                            key,
    SSL_CTX**                        ppSSLCtx
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    int                              ret = 0;
    long                             options = 0;
    const SSL_METHOD*                method = NULL;
    SSL_CTX*                         context = NULL;

    if (key == NULL || certificate == NULL)
    {
        dwError = VMREST_TRANSPORT_INVALID_PARAM;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    method = SSLv23_server_method();
    context = SSL_CTX_new(method);
    if ( context == NULL )
    {
        dwError = VMREST_TRANSPORT_SSL_CONFIG_ERROR;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    options = SSL_CTX_get_options(context);

    options = options | SSL_OP_NO_TLSv1|SSL_OP_NO_SSLv3|SSL_OP_NO_SSLv2;

    options = SSL_CTX_set_options(context, options);

    ret = SSL_CTX_set_cipher_list(context, "!aNULL:kECDH+AESGCM:ECDH+AESGCM:RSA+AESGCM:kECDH+AES:ECDH+AES:RSA+AES");
    if (ret == 0)
    {
        dwError = VMREST_TRANSPORT_SSL_INVALID_CIPHER_SUITES;
    }
    BAIL_ON_VMREST_ERROR(dwError);
 
    ret = SSL_CTX_use_certificate_file(context, certificate, SSL_FILETYPE_PEM);
    if (ret <= 0)
    {
        dwError = VMREST_TRANSPORT_SSL_CERTIFICATE_ERROR;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    ret = SSL_CTX_use_PrivateKey_file(context, key, SSL_FILETYPE_PEM);
    if (ret <= 0)
    {
        dwError = VMREST_TRANSPORT_SSL_PRIVATEKEY_ERROR;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    if (!SSL_CTX_check_private_key(context))
    {
        dwError = VMREST_TRANSPORT_SSL_PRIVATEKEY_CHECK_ERROR;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    *ppSSLCtx = context;

cleanup:
    return dwError;

error:
     dwError = VMREST_TRANSPORT_SSL_ERROR;
    goto cleanup;
}


uint32_t
VmTESTInitSSL(
    char*                            sslKey,
    char*                            sslCert,
    SSL_CTX**                        ppSSLCtx
    )
{
    uint32_t                         dwError = 0;
     
     SSL_library_init();
     dwError = VmTESTSecureSocket(
                   sslCert,
                   sslKey,
                   ppSSLCtx
                   );

     BAIL_ON_VMREST_ERROR(dwError);

     pthread_mutex_lock(&gTESTGlobalMutex);
     if (gTESTSSLisedInstaceCount == 0)
     {
         dwError = VmTESTSSLThreadLockInit();
         gTESTSSLisedInstaceCount++;
     }
     pthread_mutex_unlock(&gTESTGlobalMutex);
     BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;

error:
     dwError = VMREST_TRANSPORT_SSL_ERROR;
    goto cleanup;

}

void
VmRESTShutdownSSL(
    SSL_CTX*                         sslCtx
    )
{
     uint32_t                        destroyGlobalMutex = 0;

     if (sslCtx != NULL)
        {
            free(sslCtx);
        }

        pthread_mutex_lock(&gTESTGlobalMutex);
        gTESTSSLisedInstaceCount--;
        if (gTESTSSLisedInstaceCount == 0)
        {
            VmTESTSSLThreadLockShutdown();
            gTESTSSLThreadLock = NULL;
            destroyGlobalMutex = 1;
            gTESTSSLisedInstaceCount = -1;
        }
        pthread_mutex_unlock(&gTESTGlobalMutex);
        if (destroyGlobalMutex)
        {
            pthread_mutex_destroy(&gTESTGlobalMutex);
        }
}


