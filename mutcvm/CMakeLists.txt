cmake_minimum_required(VERSION 3.3)
project(mutcvm)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_subdirectory(frontend)
add_subdirectory(backend)

aux_source_directory(. DIR_SRCS)

set(SOURCE_FILES main.cpp ${DIR_SRCS})
add_executable(mutcvm ${SOURCE_FILES})

target_link_libraries(mutcvm backend)
target_link_libraries(mutcvm frontend)
