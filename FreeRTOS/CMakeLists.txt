cmake_minimum_required(VERSION 3.13)

# Set directory variables
set(FREERTOS_CFG_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/Config")
set(FREERTOS_SRC_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/Kernel")

# Add FreeRTOS as a library
add_library(FreeRTOS STATIC
    ${FREERTOS_SRC_DIRECTORY}/event_groups.c
    ${FREERTOS_SRC_DIRECTORY}/list.c
    ${FREERTOS_SRC_DIRECTORY}/queue.c
    ${FREERTOS_SRC_DIRECTORY}/stream_buffer.c
    ${FREERTOS_SRC_DIRECTORY}/tasks.c
    ${FREERTOS_SRC_DIRECTORY}/timers.c
    ${FREERTOS_SRC_DIRECTORY}/portable/MemMang/heap_1.c
    ${FREERTOS_SRC_DIRECTORY}/portable/ThirdParty/GCC/RP2040/port.c
)

# Add the standard library to the build
target_link_libraries(FreeRTOS
    PRIVATE pico_stdlib
    PRIVATE pico_multicore
    PRIVATE hardware_sync
    PRIVATE pico_sync
    PRIVATE hardware_exception
)

# Build FreeRTOS
target_include_directories(FreeRTOS
    PUBLIC
        ${FREERTOS_CFG_DIRECTORY}/
        ${FREERTOS_SRC_DIRECTORY}/include
        ${FREERTOS_SRC_DIRECTORY}/portable/ThirdParty/GCC/RP2040/include
)
