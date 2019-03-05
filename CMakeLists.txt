cmake_minimum_required(VERSION 2.8)
project(TX33 C)

set(LDSCRIPT ${PROJECT_SOURCE_DIR}/ldscript/stm32f103xe.ld)
set(STDLIB ${PROJECT_SOURCE_DIR}/STM32F10x_StdPeriph_Lib)
set(CORE_SUPPORT ${STDLIB}/CMSIS/CM3/CoreSupport)
set(DEVICE_SUPPORT ${STDLIB}/CMSIS/CM3/DeviceSupport/ST/STM32F10x)
set(STDLIB_INC ${STDLIB}/STM32F10x_StdPeriph_Driver/inc)
set(STDLIB_SRC ${STDLIB}/STM32F10x_StdPeriph_Driver/src)

add_compile_options(-W -Wall -g)
add_link_options(-T ${LDSCRIPT} -Wl,-Map=${PROJECT_NAME}.map -Wl,--start-group -Wl,--end-group -specs=nano.specs -specs=nosys.specs -static -Wl,-cref,-u,Reset_Handler -Wl,--gc-sections -Wl,--defsym=malloc_getpagesize_P=0x80)

include_directories(inc ${STDLIB_INC} ${CORE_SUPPORT} ${DEVICE_SUPPORT})
add_definitions(-DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER)
aux_source_directory(src SRC_LIST)
set(SRC_LIST
  ${SRC_LIST}
  ${DEVICE_SUPPORT}/system_stm32f10x.c
  ${STDLIB_SRC}/misc.c
  ${STDLIB_SRC}/stm32f10x_exti.c
  ${STDLIB_SRC}/stm32f10x_gpio.c
  ${STDLIB_SRC}/stm32f10x_rcc.c
  ${STDLIB_SRC}/stm32f10x_tim.c
  )

set(CMAKE_EXECUTABLE_SUFFIX .elf)
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} c m)

add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
  COMMAND ${CMAKE_OBJCOPY} -O ihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMENT "Generating binary and hexadecimal files"
  )

if(CMAKE_EXPORT_COMPILE_COMMANDS)  
  add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -D PROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR} -P ${PROJECT_SOURCE_DIR}/copy-compile_commands.cmake
    )
endif()  

add_custom_command(
  OUTPUT ${PROJECT_NAME}.bin 
  COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
  DEPENDS ${PROJECT_NAME}
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMENT "Generating binary file"
  )

add_custom_command(
  OUTPUT ${PROJECT_NAME}.hex
  COMMAND ${CMAKE_OBJCOPY} -O ihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
  DEPENDS ${PROJECT_NAME}
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMENT "Generating hexadecimal file"
  )

add_custom_target(
  download
  openocd -f openocd.cfg -c init -c halt -c flash write_image erase ${PROJECT_NAME}.hex
  DEPENDS ${PROJECT_NAME}.hex
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMENT "Downloading hexadecimal file"
  )

install(
  TARGETS ${PROJECT_NAME}
  DESTINATION ${PROJECT_SOURCE_DIR}
  )

install(
  FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.hex
  DESTINATION ${PROJECT_SOURCE_DIR}
  )
