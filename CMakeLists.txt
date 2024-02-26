cmake_minimum_required(VERSION 3.5)
project(loris)

file(GLOB loris_src src/*.C)
list(APPEND source_files ${loris_src})
file(GLOB loris_include src/*.h)
list(APPEND include_files ${loris_include})

add_definitions(-DHAVE_FFTW3_H)
set(libs m fftw3)

include_directories(src)

add_library(loris SHARED ${source_files})
target_link_libraries(loris ${libs})

install(TARGETS loris LIBRARY DESTINATION lib)



