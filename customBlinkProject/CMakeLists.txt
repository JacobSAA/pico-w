cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(customBlinkProject C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

add_executable(customBlink
    customBlink.c
)

target_link_libraries(customBlink
pico_stdlib              # for core functionality
pico_cyw43_arch_none     # we need Wifi to access the GPIO, but we don't need anything else
)

# enable usb output, disable uart output
pico_enable_stdio_usb(customBlink 1)
pico_enable_stdio_uart(customBlink 0)

pico_add_extra_outputs(customBlink)