cmake_minimum_required(VERSION 3.5)
project(loris)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/loris.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/loris.h @ONLY)

#╭──────────────────────────────────────╮
#│             Static FFTW3             │
#╰──────────────────────────────────────╯

cmake_policy(SET CMP0135 NEW)
option(BUILD_SHARED_LIBS OFF)
option(BUILD_TESTS OFF)
option(DISABLE_FORTRAN ON)

# check if fftw-3.3.10.tar.gz has been downloaded
set(FFTW3_FILE ${CMAKE_BINARY_DIR}/fftw-3.3.10.tar.gz)
if (NOT EXISTS ${FFTW3_FILE})
    message(STATUS "Downloading FFTW3")
    file(DOWNLOAD https://www.fftw.org/fftw-3.3.10.tar.gz ${FFTW3_FILE})
endif()

file(ARCHIVE_EXTRACT
    INPUT ${CMAKE_BINARY_DIR}/fftw-3.3.10.tar.gz
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/
)

add_subdirectory(Libraries/fftw-3.3.10)
set_target_properties(fftw3 PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set_target_properties(fftw3 PROPERTIES POSITION_INDEPENDENT_CODE ON)



file(GLOB SOURCES "src/*.C")
add_library(loris ${SOURCES})

# compile using C++
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++03")

add_definitions(-DHAVE_FFTW3_H)
target_include_directories(loris PUBLIC include)
target_link_libraries(loris fftw3)
