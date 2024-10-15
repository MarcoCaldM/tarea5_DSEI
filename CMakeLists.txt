# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)



include($ENV{IDF_PATH}/tools/cmake/project.cmake)

#Definiciones de compilacion
if(${IDF_TARGET} STREQUAL "esp32")
    add_compile_definitions(ESP32)
elseif(${IDF_TARGET} STREQUAL "esp32c3")
    add_compile_definitions(ESP32C3)
endif()

add_compile_definitions(RTOS=1)

project(DSE_Tarea2_Equipo1)
