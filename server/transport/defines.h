/* provide port and ip information here for the time being */
#define PORT 61001
#define SERVERIP 127.0.0.1
#define MAX_EVENT 64


#define MAX_ADDRESS_LEN 128
#define MAX_PORT_LEN 6

#define ERROR_NOT_SUPPORTED 100

#define BAIL_ON_POSIX_SOCK_ERROR(dwError) \
        if (dwError) \
            goto error;
