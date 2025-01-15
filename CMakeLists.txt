cmake_minimum_required(VERSION 3.25)
project(loris)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/loris.h.in
               ${CMAKE_CURRENT_SOURCE_DIR}/src/loris.h @ONLY)

if(MSVC)
  set(CMAKE_C_COMPILER "gcc")
endif()

option(PYTHON "Build Python Wrapper" ON)
option(LUA "Build Lua Wrapper" OFF)

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
target_link_libraries(loris PUBLIC fftw3)
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
if(PYTHON)
  message(STATUS "Python Wrapper Enabled")
  find_package(SWIG REQUIRED)
  include(${SWIG_USE_FILE})
  set(SKBUILD_PROJECT_NAME "_loris") # Or any suitable path

  set(SWIG_INTERFACE ${CMAKE_SOURCE_DIR}/scripting/loris.i)
  set(PYTHON_WRAPPER ${CMAKE_BINARY_DIR}/loris_py.cxx)

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
                                            ${CMAKE_SOURCE_DIR}/src)

  target_link_libraries(_loris PRIVATE ${Python3_LIBRARIES} loris)
  add_dependencies(_loris GeneratePythonWrapper)

  install(TARGETS _loris DESTINATION loris)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/loris.py DESTINATION loris)
  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/scripting/__init__.py DESTINATION loris)
endif()

# ╭──────────────────────────────────────╮
# │           SWIG Lua Wrapper           │
# ╰──────────────────────────────────────╯
# Ensure LUA_WRAPPER is defined
if(LUA)
  find_package(SWIG REQUIRED)
  set(SWIG_INTERFACE ${CMAKE_SOURCE_DIR}/scripting/loris.i)
  set(LUA_WRAPPER ${CMAKE_BINARY_DIR}/loris_lua.cxx)

  # Get the SWIG library path
  execute_process(
    COMMAND ${SWIG_EXECUTABLE} -swiglib
    OUTPUT_VARIABLE SWIG_LIB_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  if(SWIG_LIB_DIR)
    set(CMAKE_SWIG_FLAGS "-I${SWIG_LIB_DIR}/std")
    message(STATUS "Using SWIG Standard Library Path: ${SWIG_LIB_DIR}/std")
  else()
    message(FATAL_ERROR "Failed to determine SWIG standard library path.")
  endif()

  if(NOT SWIG_INTERFACE)
    message(FATAL_ERROR "SWIG_INTERFACE is not defined.")
  endif()

  add_custom_command(
    OUTPUT ${LUA_WRAPPER}
    COMMAND ${SWIG_EXECUTABLE} -lua -c++ -module loris -o ${LUA_WRAPPER}
            ${SWIG_INTERFACE}
    DEPENDS ${SWIG_INTERFACE}
    COMMENT "Generating Lua wrapper using SWIG")

  message(
    STATUS
      "${SWIG_EXECUTABLE} -lua -c++ -module loris -o ${LUA_WRAPPER} ${SWIG_INTERFACE}"
  )
  # Custom target for Lua wrapper generation
  add_custom_target(GenerateLuaWrapper ALL DEPENDS ${LUA_WRAPPER})

  # Find Lua
  find_package(Lua REQUIRED)

  if(NOT LUA_INCLUDE_DIR OR NOT LUA_LIBRARIES)
    message(FATAL_ERROR "Lua package not found or incomplete.")
  endif()

  # Create the shared library

  add_library(lorislua SHARED ${LUA_WRAPPER})
  set_target_properties(lorislua PROPERTIES PREFIX "" OUTPUT_NAME "loris")
  target_include_directories(lorislua PRIVATE ${Python3_INCLUDE_DIRS}
                                              ${CMAKE_CURRENT_SOURCE_DIR}/src)

  # Include Lua directories and link libraries
  target_include_directories(lorislua PRIVATE ${LUA_INCLUDE_DIR})
  target_link_libraries(lorislua PRIVATE ${LUA_LIBRARIES})
  target_link_libraries(lorislua PRIVATE loris)

  # Ensure dependency order
  add_dependencies(lorislua GenerateLuaWrapper)

endif()
