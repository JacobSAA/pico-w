cd build
set PICO_SDK_PATH=../../pico-sdk
cmake .. -DPICO_BOARD=pico_w -G "MinGW Makefiles"
make -j4
cd ..
cmd /k