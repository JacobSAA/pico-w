cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(buttonInterface C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

add_executable(buttonInterface
    buttonInterface.c
    apicall/apiCall.c
)

# enable usb output, disable uart output
pico_enable_stdio_usb(buttonInterface 1)
pico_enable_stdio_uart(buttonInterface 0)


pico_add_extra_outputs(buttonInterface)

target_include_directories(buttonInterface PRIVATE 
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(buttonInterface
pico_stdlib              # for core functionality
pico_cyw43_arch_lwip_threadsafe_background    
pico_lwip_http
)