cmake_minimum_required (VERSION 2.8.11)
project (PROFILING)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose debug or release" FORCE)
endif(NOT CMAKE_BUILD_TYPE)

set(MANGO_ROOT /opt/mango)
set(INCLUDE_DIR ${MANGO_ROOT}/include/profiling)
set(LIB_DIR ${MANGO_ROOT}/lib/profiling)
set(EXPORT_DIR ${MANGO_ROOT}/lib/cmake/profiling)

add_library(profiling SHARED profiling.cpp)

target_include_directories(profiling PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${INCLUDE_DIR}>
)

install(FILES profiling.h DESTINATION ${INCLUDE_DIR})
install(TARGETS profiling EXPORT profilingConfig LIBRARY DESTINATION ${LIB_DIR})

install(
    EXPORT profilingConfig 
    NAMESPACE profiling::
    DESTINATION ${EXPORT_DIR}
)
