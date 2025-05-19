# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PICO_BOARD pico_w CACHE STRING "Board type")
include(pico_sdk_import.cmake)

set(FREERTOS_KERNEL_PATH "C:/RTOS/FreeRTOS-Kernel")
set(FREERTOS_PORT GCC_RP2040)
set(FREERTOS_HEAP 4)
include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

project(main C CXX ASM)
pico_sdk_init()

add_executable(${PROJECT_NAME} main.c )

pico_enable_stdio_uart(${PROJECT_NAME} 1)
pico_enable_stdio_usb(${PROJECT_NAME} 1)

pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio)

target_include_directories(${PROJECT_NAME} PRIVATE
    lib
)

target_sources(${PROJECT_NAME} PRIVATE 
    lib/led.c
    lib/push_button.c
    lib/pwm.c
    lib/ssd1306.c
    lib/ws2812b.c
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    pico_stdlib
    hardware_clocks
    hardware_timer
    hardware_pwm
    hardware_i2c
    hardware_pio
    hardware_gpio
    hardware_adc
    FreeRTOS-Kernel 
    FreeRTOS-Kernel-Heap4
)
pico_add_extra_outputs(${PROJECT_NAME})