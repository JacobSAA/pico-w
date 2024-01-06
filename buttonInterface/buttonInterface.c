#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "boards/pico_w.h"
#include "lwip/apps/http_client.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "apicall/apiCall.h"

#define BUTTON_PIN 18

char ssid[] = "Tell my WiFi love her";
char pass[] = "WhatsItToYa";
uint32_t country = CYW43_COUNTRY_USA;
uint32_t auth = CYW43_AUTH_WPA2_MIXED_PSK;

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

    // initDebug();
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    ip_addr_t ip;
    IP4_ADDR(&ip, 192,168,0,250);

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    while (true) {
        if (!gpio_get(BUTTON_PIN)) {
            if (makeApiCallV2("/question", ip, 5000)) {
                printf("API Call Started\n");
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