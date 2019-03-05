set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# set(CMAKE_C_COMPILER C:/GNU/gcc-arm-none-eabi/bin/arm-none-eabi-gcc.exe)
# set(CMAKE_CXX_COMPILER C:/GNU/gcc-arm-none-eabi/bin/arm-none-eabi-g++.exe)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_C_COMPILER_ID GNU)
set(CMAKE_CXX_COMPILER_ID GNU)
# set(CMAKE_OBJCOPY C:/GNU/gcc-arm-none-eabi/bin/arm-none-eabi-objcopy.exe)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)

set(CMAKE_C_FLAGS_INIT "-mcpu=cortex-m3 -mthumb")
set(CMAKE_CXX_FLAGS_INIT "-mcpu=cortex-m3 -mthumb")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-mthumb -mcpu=cortex-m3 -nostartfiles")
