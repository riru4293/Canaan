cmake_minimum_required(VERSION 3.13)

# C standard features required for building
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build type
set(CMAKE_BUILD_TYPE Debug)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
include(${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
# ====================================================================================

# Specify the target board (must be before pico_sdk_init)
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

# Define the project name
project(Canaan
    VERSION 1.0.0
    DESCRIPTION "UART receive to CAN send and CAN receive to UART send."
    LANGUAGES CXX C ASM
)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Include the sub modules
add_subdirectory(FreeRTOS)

add_executable(Canaan main.c)

# Add the standard library to the build
target_link_libraries(Canaan
    PRIVATE pico_stdlib
    PRIVATE hardware_uart
    PRIVATE FreeRTOS
)

pico_set_program_name(Canaan "Canaan")
pico_set_program_version(Canaan "1.0.0")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(Canaan 1)
pico_enable_stdio_usb(Canaan 0)

pico_add_extra_outputs(Canaan)
