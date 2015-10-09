cmake_minimum_required(VERSION 3.2)
project(mutcc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


add_subdirectory(frontend)
add_subdirectory(backend)

set(SOURCE_FILES main.cpp)
add_executable(mutcc ${SOURCE_FILES})

target_link_libraries(mutcc frontend)
target_link_libraries(mutcc backend)


