cmake_minimum_required(VERSION 3.22)

# --------------------------------------------
# Project configuration
# --------------------------------------------
set(CMAKE_PROJECT_NAME Projet_ESE_chat_robot)
project(${CMAKE_PROJECT_NAME} C ASM)

# C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()
message("Build type: ${CMAKE_BUILD_TYPE}")

# Generate compile_commands.json (useful for clangd / IntelliSense)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# --------------------------------------------
# Toolchain (ARM GCC for STM32L476RG)
# --------------------------------------------
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

# MCU & FPU settings (Cortex-M4 with hardware FPU)
set(MCU_FLAGS "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard")

# Common compiler flags
set(CMAKE_C_FLAGS "${MCU_FLAGS} -O0 -g3 -Wall -ffunction-sections -fdata-sections")
set(CMAKE_ASM_FLAGS "${MCU_FLAGS} -g3")
set(CMAKE_EXE_LINKER_FLAGS "${MCU_FLAGS} -T${CMAKE_SOURCE_DIR}/STM32L476XX_FLASH.ld -Wl,-Map=${CMAKE_PROJECT_NAME}.map -Wl,--gc-sections")

# --------------------------------------------
# Source files
# --------------------------------------------
file(GLOB_RECURSE SOURCES
    "${CMAKE_SOURCE_DIR}/Core/Src/*.c"
    "${CMAKE_SOURCE_DIR}/Core/Startup/*.s"        # <-- ajoute les fichiers startup ASM
    "${CMAKE_SOURCE_DIR}/Drivers/STM32L4xx_HAL_Driver/Src/*.c"
    "${CMAKE_SOURCE_DIR}/Drivers/CMSIS/RTOS2/Template/*.c"
    "${CMAKE_SOURCE_DIR}/Drivers/CMSIS/RTOS2/Source/*.c"
    "${CMAKE_SOURCE_DIR}/LED/led.c"
    "${CMAKE_SOURCE_DIR}/MP3/mp3.c"
    "${CMAKE_SOURCE_DIR}/TOF/tof.c"
    "${CMAKE_SOURCE_DIR}/Rasbpi/rasbpi.c"
    "${CMAKE_SOURCE_DIR}/LiDAR/lidar.c"
    "${CMAKE_SOURCE_DIR}/TOF/VL53L1X_api.c"
    "${CMAKE_SOURCE_DIR}/TOF/vl53l1_platform.c"
    "${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/cmsis_os2.c"
    "${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c"
    "${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/*.c"

)

# --------------------------------------------
# Executable target
# --------------------------------------------
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

# --------------------------------------------
# Include directories
# --------------------------------------------
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/Core/Inc"
    "${CMAKE_SOURCE_DIR}/Drivers/STM32L4xx_HAL_Driver/Inc"
    "${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include"
    "${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32L4xx/Include"
    "${CMAKE_SOURCE_DIR}/Drivers/CMSIS/RTOS2/Template"
    "${CMAKE_SOURCE_DIR}/Drivers/CMSIS/RTOS2/Source"
    "${CMAKE_SOURCE_DIR}/LED"
    "${CMAKE_SOURCE_DIR}/MP3"
    "${CMAKE_SOURCE_DIR}/TOF"
    "${CMAKE_SOURCE_DIR}/Rasbpi"
    "${CMAKE_SOURCE_DIR}/LiDAR"
    "${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS"
    "${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/include"
    "${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F"


)

# --------------------------------------------
# Preprocessor definitions
# --------------------------------------------
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    USE_HAL_DRIVER
    STM32L476xx
)

# --------------------------------------------
# Optional linked libraries
# --------------------------------------------
# target_link_libraries(${CMAKE_PROJECT_NAME}
#     m
# )

# Remove incorrect libob.a dependency (Cube quirk)
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# --------------------------------------------
# End of CMakeLists.txt
# --------------------------------------------
