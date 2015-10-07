#include "defines.h"
typedef struct _VM_SOCKET
{
    int fd;
    char address[MAX_ADDRESS_LEN];
    char port[MAX_PORT_LEN];

} VM_SOCKET;

