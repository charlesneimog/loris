cmake_minimum_required(VERSION 3.5)
project(loris)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/loris.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/loris.h @ONLY)

add_definitions(-DHAVE_FFTW3_H)
set(libs m fftw3)

file(GLOB SOURCES "src/*.C")
add_library(loris ${SOURCES})

target_include_directories(loris PUBLIC include)
