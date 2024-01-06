#include "lwip/tcp.h"

#define BUF_SIZE 2048

typedef struct TCP_CLIENT_T_ {
    struct tcp_pcb *tcp_pcb;
    ip_addr_t remote_addr;
    char *endpoint;
    uint8_t buffer[BUF_SIZE];
} TCP_CLIENT_T;

bool makeApiCallV2(char *endpoint, ip_addr_t ip, u16_t port);