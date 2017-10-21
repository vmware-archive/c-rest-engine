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

static
void
VmRESTSSLThreadLockCallback(
    int                              mode,
    int                              type,
    char*                            file,
    int                              line
    );

static
unsigned long
VmRESTSSLThreadId(
    void
    );

static
void
VmRESTSSLThreadLockCallback(
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
        pthread_mutex_lock(&(gSSLThreadLock[type]));
    }
    else
    {
        pthread_mutex_unlock(&(gSSLThreadLock[type]));
    }
}

static
unsigned long
VmRESTSSLThreadId(
    void
    )
{
    unsigned long                      ret = 0;

    ret = (unsigned long)pthread_self();
    return ret;
}

uint32_t
VmRESTSSLThreadLockInit(
    void
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    int                              i = 0;

    gSSLThreadLock = (pthread_mutex_t *)OPENSSL_malloc(
                                           CRYPTO_num_locks() * sizeof(pthread_mutex_t)
                                           );
    if (gSSLThreadLock == NULL)
    {
        dwError = REST_ERROR_NO_MEMORY;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    for(i = 0; i < CRYPTO_num_locks(); i++) 
    {
        pthread_mutex_init(&(gSSLThreadLock[i]), NULL);
    }

    CRYPTO_set_id_callback((unsigned long (*)())VmRESTSSLThreadId);
    CRYPTO_set_locking_callback((void (*)())VmRESTSSLThreadLockCallback);

cleanup:
    return dwError;
error:
    dwError = VMREST_TRANSPORT_SSL_ERROR;
    goto cleanup;
}

void
VmRESTSSLThreadLockShutdown(
    void
    )
{
    int                              i = 0;

    CRYPTO_set_locking_callback(NULL);

    for( i = 0; i < CRYPTO_num_locks(); i++)
    {
        pthread_mutex_destroy(&(gSSLThreadLock[i]));
    }
    OPENSSL_free(gSSLThreadLock);
}

uint32_t
VmRESTSecureSocket(
    PVMREST_HANDLE                   pRESTHandle,
    char*                            certificate,
    char*                            key
    )
{
    uint32_t                         dwError = REST_ENGINE_SUCCESS;
    int                              ret = 0;
    long                             options = 0;
    const SSL_METHOD*                method = NULL;
    SSL_CTX*                         context = NULL;

    if (key == NULL || certificate == NULL)
    {
        VMREST_LOG_ERROR(pRESTHandle,"Invalid params");
        dwError = VMREST_TRANSPORT_INVALID_PARAM;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    method = SSLv23_server_method();
    context = SSL_CTX_new(method);
    if ( context == NULL )
    {
        VMREST_LOG_ERROR(pRESTHandle,"SSL context is NULL");
        dwError = VMREST_TRANSPORT_SSL_CONFIG_ERROR;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    options = SSL_CTX_get_options(context);

    options = options | SSL_OP_NO_TLSv1|SSL_OP_NO_SSLv3|SSL_OP_NO_SSLv2;

    options = SSL_CTX_set_options(context, options);

    ret = SSL_CTX_set_cipher_list(context, "!aNULL:kECDH+AESGCM:ECDH+AESGCM:RSA+AESGCM:kECDH+AES:ECDH+AES:RSA+AES");
    if (ret == 0)
    {
        VMREST_LOG_ERROR(pRESTHandle,"SSL_CTX_set_cipher_list() : Cannot apply security approved cipher suites");
        dwError = VMREST_TRANSPORT_SSL_INVALID_CIPHER_SUITES;
    }
    BAIL_ON_VMREST_ERROR(dwError);
 
    ret = SSL_CTX_use_certificate_file(context, certificate, SSL_FILETYPE_PEM);
    if (ret <= 0)
    {
        VMREST_LOG_ERROR(pRESTHandle,"Cannot Use SSL certificate");
        dwError = VMREST_TRANSPORT_SSL_CERTIFICATE_ERROR;
    }
    BAIL_ON_VMREST_ERROR(dwError);

    ret = SSL_CTX_use_PrivateKey_file(context, key, SSL_FILETYPE_PEM);
    if (ret <= 0)
    {
        VMREST_LOG_ERROR(pRESTHandle,"Cannot use private key file");
        dwError = VMREST_TRANSPORT_SSL_PRIVATEKEY_ERROR;
        BAIL_ON_VMREST_ERROR(dwError);
    }
    if (!SSL_CTX_check_private_key(context))
    {
        VMREST_LOG_ERROR(pRESTHandle,"Error in Private Key");
        dwError = VMREST_TRANSPORT_SSL_PRIVATEKEY_CHECK_ERROR;
        BAIL_ON_VMREST_ERROR(dwError);
    }

    pRESTHandle->pSSLInfo->sslContext = context;

cleanup:
    return dwError;

error:
     dwError = VMREST_TRANSPORT_SSL_ERROR;
    goto cleanup;
}
