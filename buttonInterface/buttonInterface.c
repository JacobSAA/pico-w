#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "boards/pico_w.h"
#include "lwip/apps/http_client.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"

#define BUTTON_PIN 18
#define BUF_SIZE 2048

typedef struct TCP_CLIENT_T_ {
    struct tcp_pcb *tcp_pcb;
    ip_addr_t remote_addr;
    char *endpoint;
    uint8_t buffer[BUF_SIZE];
} TCP_CLIENT_T;

char ssid[] = "Tell my WiFi love her";
char pass[] = "WhatsItToYa";
uint32_t country = CYW43_COUNTRY_USA;
uint32_t auth = CYW43_AUTH_WPA2_MIXED_PSK;

char myBuff[2000] = "";

bool apiCallInProgress = false;

int setup(uint32_t country, const char *ssid, 
    const char *pass, uint32_t auth, 
    const char *hostname, ip_addr_t *ip,
                   ip_addr_t *mask, ip_addr_t *gw)
{
    if (cyw43_arch_init_with_country(country))
    {
        return 1;
    }

    cyw43_arch_enable_sta_mode();
    if (hostname != NULL)
    {
        netif_set_hostname(netif_default, hostname);
    }


    printf("Connecting to Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(ssid, pass, auth, 10000))
    {
        printf("Failed to connect.\n");
        return 2;
    }

    return 0;
}

err_t body(void *arg, struct altcp_pcb *conn, struct pbuf *p, err_t err)
{
    printf("body\n");
    pbuf_copy_partial(p, myBuff, p->tot_len, 0);
    // printf("%s", myBuff);
    // pbuf_free(p);
    return ERR_OK;
}

err_t headers(httpc_state_t *connection, void *arg, struct pbuf *hdr, u16_t hdr_len, u32_t content_len)
{
    printf("headers recieved\n");
    printf("content length=%d\n", content_len);
    printf("header length %d\n", hdr_len);
    printf("total length=%d\n", hdr->tot_len);
    int copied = pbuf_copy_partial(hdr, myBuff, hdr->tot_len, 0);
    printf("copied=%d\n", copied);
    printf("headers \n");
    for(int i = 0; i < 2000; i++) {
        printf("%c", myBuff[i]);
    }
    printf("\n");
    // printf("%s", myBuff);
    // pbuf_free(hdr);
    return ERR_OK;
}

void result (void *arg, httpc_result_t httpc_result, u32_t rx_content_len, u32_t srv_res, err_t err)
{
    // apiCallInProgress = false;
    printf("transfer complete\n");
    printf("local result=%d\n", httpc_result);
    printf("http result=%d\n", srv_res);
}

err_t makeApiCall(char *endpoint, ip_addr_t ip, u16_t port) {
    // apiCallInProgress = true;
    httpc_connection_t settings;
    settings.result_fn = result;
    settings.headers_done_fn = headers;
    return httpc_get_file(
                &ip,
                port,
                endpoint,
                &settings,
                body,
                NULL,
                NULL
            ); 

}

static void dump_bytes(const uint8_t *bptr, uint32_t len) {
    unsigned int i = 0;

    printf("dump_bytes %d", len);
    for (i = 0; i < len;) {
        if ((i & 0x0f) == 0) {
            printf("\n");
        } else if ((i & 0x07) == 0) {
            printf(" ");
        }
        printf("%02x ", bptr[i++]);
    }
    printf("\n");
}

static err_t tcp_client_close(void *arg) {
    TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
    err_t err = ERR_OK;
    if (state->tcp_pcb != NULL) {
        tcp_arg(state->tcp_pcb, NULL);
        tcp_sent(state->tcp_pcb, NULL);
        tcp_recv(state->tcp_pcb, NULL);
        tcp_err(state->tcp_pcb, NULL);
        err = tcp_close(state->tcp_pcb);
        if (err != ERR_OK) {
            printf("close failed %d, calling abort\n", err);
            tcp_abort(state->tcp_pcb);
            err = ERR_ABRT;
        }
        state->tcp_pcb = NULL;
    }
    return err;
}

// Called with results of operation
static err_t tcp_result(void *arg, int status) {
    TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
    apiCallInProgress = false;
    if (status == 0) {
        printf("test success\n");
    } else {
        printf("test failed %d\n", status);
    }
    return tcp_client_close(arg);
}

void tcp_send_packet(void *arg, struct tcp_pcb *tpcb) {
    printf("Writing to server\n");
    char *string = "GET /attention HTTP/1.1\r\nHost: 192.168.0.250:5000\r\n\r\n";
    err_t err = tcp_write(tpcb, string, strlen(string), TCP_WRITE_FLAG_COPY);
    if (err != ERR_OK) {
        printf("Failed to write data %d\n", err);
        tcp_result(arg, -1);
    }
}

static err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err) {
    TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
    if (err != ERR_OK) {
        printf("connect failed %d\n", err);
        return tcp_result(arg, err);
    }
    tcp_send_packet(arg, tpcb);
    return ERR_OK;
}

err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
    if (!p) {
        printf("No pbuf ERROR");
        return tcp_result(arg, -1);
    }
    // this method is callback from lwIP, so cyw43_arch_lwip_begin is not required, however you
    // can use this method to cause an assertion in debug mode, if this method is called when
    // cyw43_arch_lwip_begin IS needed
    cyw43_arch_lwip_check();
    if (p->tot_len > 0) {
        printf("recv %d err %d\n", p->tot_len, err);
        for (struct pbuf *q = p; q != NULL; q = q->next) {
            dump_bytes(q->payload, q->len);
        }
        // Receive the buffer
        int len = pbuf_copy_partial(p, state->buffer, p->tot_len, 0);
        tcp_recved(tpcb, p->tot_len);
    }
    pbuf_free(p);

        // DEBUG_printf("Writing %d bytes to server\n", state->buffer_len);
        // err_t err = tcp_write(tpcb, state->buffer, state->buffer_len, TCP_WRITE_FLAG_COPY);
        // if (err != ERR_OK) {
        //     DEBUG_printf("Failed to write data %d\n", err);
        //     return tcp_result(arg, -1);
        // }
    return ERR_OK;
}

static void tcp_client_err(void *arg, err_t err) {
    if (err != ERR_ABRT) {
        printf("tcp_client_err %d\n", err);
        tcp_result(arg, err);
    }
}

static err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    printf("tcp_client_sent %u\n", len);
    return ERR_OK;
}

static bool tcp_client_open(void *arg, u16_t port) {
    TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
    printf("Connecting to %s port %u\n", ip4addr_ntoa(&state->remote_addr), port);
    state->tcp_pcb = tcp_new_ip_type(IP_GET_TYPE(&state->remote_addr));
    if (!state->tcp_pcb) {
        printf("failed to create pcb\n");
        return false;
    }

    tcp_arg(state->tcp_pcb, state);
    tcp_sent(state->tcp_pcb, tcp_client_sent);
    tcp_recv(state->tcp_pcb, tcp_client_recv);
    tcp_err(state->tcp_pcb, tcp_client_err);

    // cyw43_arch_lwip_begin/end should be used around calls into lwIP to ensure correct locking.
    // You can omit them if you are in a callback from lwIP. Note that when using pico_cyw_arch_poll
    // these calls are a no-op and can be omitted, but it is a good practice to use them in
    // case you switch the cyw43_arch type later.
    cyw43_arch_lwip_begin();
    err_t err = tcp_connect(state->tcp_pcb, &state->remote_addr, port, tcp_client_connected);
    cyw43_arch_lwip_end();

    return err == ERR_OK;
}

// Perform initialisation
static TCP_CLIENT_T* tcp_client_init(ip_addr_t ip) {
    TCP_CLIENT_T *state = calloc(1, sizeof(TCP_CLIENT_T));
    if (!state) {
        printf("failed to allocate state\n");
        return NULL;
    }
    state -> remote_addr = ip;
    return state;
}

void makeApiCallV2(char *endpoint, ip_addr_t ip, u16_t port) {
    // apiCallInProgress = true;
    TCP_CLIENT_T *state = tcp_client_init(ip);
    if (!state) {
        printf("State failed to load\n");
        return;
    }
    apiCallInProgress = true;
    if (!tcp_client_open(state, port)) {
        tcp_result(state, -1);
        return;
    }
}

/*
* Turn on led to indicate ready to debug. Wait 20 seconds to allow for connect before debug
*/
void initDebug() {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    printf("LED ON!\n");
    int debugTime = 5;
    for (int i = 0; i < debugTime; i++) {
        printf("Debug: %d\n", (debugTime - i));
        sleep_ms(1000);
    }
}

int main() {
    // BAUD 115200
    // use cmd mode to get list of serial ports
    stdio_init_all();

    
    setup(country, ssid, pass, auth, 
               "MyPicoW", NULL, NULL, NULL);
    printf("Connected.\n");

    initDebug();

    ip_addr_t ip;
    IP4_ADDR(&ip, 192,168,0,250);

    makeApiCallV2("/attention", ip, 5000);

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    // err_t err; 
    while (true) {
        if (!gpio_get(BUTTON_PIN)) {
            if (!apiCallInProgress) {
                makeApiCallV2("/attention", ip, 5000);
                // err = makeApiCall("/attention", ip, 5000);
                // printf("status %d \n", err);
            }
            printf("button TRUE\n");
            sleep_ms(500);
        }

        // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        // // printf("LED ON!\n");
        // sleep_ms(500);
        // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        // // printf("LED OFF !\n");
        // sleep_ms(500);
    }
}