# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Workspace/Pico/pico-sdk/tools/pioasm"
  "C:/Workspace/Pico/buttonInterface/build/pioasm"
  "C:/Workspace/Pico/buttonInterface/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm"
  "C:/Workspace/Pico/buttonInterface/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/tmp"
  "C:/Workspace/Pico/buttonInterface/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/PioasmBuild-stamp"
  "C:/Workspace/Pico/buttonInterface/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src"
  "C:/Workspace/Pico/buttonInterface/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/PioasmBuild-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Workspace/Pico/buttonInterface/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/PioasmBuild-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Workspace/Pico/buttonInterface/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/PioasmBuild-stamp${cfgdir}") # cfgdir has leading slash
endif()
