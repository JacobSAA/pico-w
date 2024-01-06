#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "boards/pico_w.h"

int main() {
    // BAUD 115200
    // use cmd mode to get list of serial ports
    stdio_init_all();
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed");
        return -1;
    }
    while (true) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        printf("LED ON!\n");
        sleep_ms(250);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        printf("LED OFFn !\n");
        sleep_ms(250);
    }
}