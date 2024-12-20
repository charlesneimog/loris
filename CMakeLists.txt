cmake_minimum_required(VERSION 3.25)
project(loris)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/loris.h.in
               ${CMAKE_CURRENT_SOURCE_DIR}/src/loris.h @ONLY)

if(MSVC)
  set(CMAKE_C_COMPILER "gcc")
endif()

# ╭──────────────────────────────────────╮
# │             Static FFTW3             │
# ╰──────────────────────────────────────╯
cmake_policy(SET CMP0135 NEW)
option(BUILD_SHARED_LIBS OFF)
option(BUILD_TESTS OFF)
set(FFTW3_FILE ${CMAKE_BINARY_DIR}/fftw-3.3.10.tar.gz)
if(NOT EXISTS ${FFTW3_FILE})
  message(STATUS "Downloading FFTW3")
  file(DOWNLOAD https://www.fftw.org/fftw-3.3.10.tar.gz ${FFTW3_FILE})
endif()

file(ARCHIVE_EXTRACT INPUT ${CMAKE_BINARY_DIR}/fftw-3.3.10.tar.gz DESTINATION
     ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/)

add_subdirectory(Libraries/fftw-3.3.10 EXCLUDE_FROM_ALL)
set_target_properties(fftw3 PROPERTIES LIBRARY_OUTPUT_DIRECTORY
                                       ${CMAKE_SOURCE_DIR})
set_target_properties(fftw3 PROPERTIES POSITION_INDEPENDENT_CODE ON)

# ╭──────────────────────────────────────╮
# │             Static Loris             │
# ╰──────────────────────────────────────╯
file(GLOB SOURCES "src/*.C")
add_library(loris STATIC ${SOURCES})
target_include_directories(
  loris PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/fftw-3.3.10/api)

add_definitions(-DHAVE_FFTW3_H)
target_include_directories(loris PUBLIC include ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(loris fftw3)
set_target_properties(loris PROPERTIES POSITION_INDEPENDENT_CODE ON)

if(LINUX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++03")
elseif(APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++03")
elseif(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++03")
endif()

# ╭──────────────────────────────────────╮
# │         SWIG Python Wrapper          │
# ╰──────────────────────────────────────╯
find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

set(SWIG_INTERFACE ${CMAKE_SOURCE_DIR}/scripting/loris.i)
set(PYTHON_WRAPPER ${CMAKE_BINARY_DIR}/loris_wrap.cxx)

add_custom_command(
  OUTPUT ${PYTHON_WRAPPER}
  COMMAND ${SWIG_EXECUTABLE} -python -c++ -module loris -o ${PYTHON_WRAPPER}
          ${SWIG_INTERFACE}
  DEPENDS ${SWIG_INTERFACE}
  COMMENT "Generating Python wrapper using SWIG")

# Criar um alvo (target) customizado para acionar a geração do wrapper
add_custom_target(GeneratePythonWrapper ALL DEPENDS ${PYTHON_WRAPPER})
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
add_library(_loris SHARED ${PYTHON_WRAPPER})
set_target_properties(_loris PROPERTIES PREFIX "" OUTPUT_NAME "_loris")

target_include_directories(_loris PRIVATE ${Python3_INCLUDE_DIRS}
                                          ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(_loris PRIVATE ${Python3_LIBRARIES} loris)
add_dependencies(_loris GeneratePythonWrapper)

# ╭──────────────────────────────────────╮
# │               Install                │
# ╰──────────────────────────────────────╯
install(TARGETS _loris DESTINATION ${SKBUILD_PROJECT_NAME})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/loris.py
        DESTINATION ${SKBUILD_PROJECT_NAME})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/scripting/__init__.py
        DESTINATION ${SKBUILD_PROJECT_NAME})
