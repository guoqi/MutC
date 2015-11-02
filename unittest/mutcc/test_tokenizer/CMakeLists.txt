cmake_minimum_required(VERSION 3.2)
project(test_tokenizer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_subdirectory(frontend)
add_subdirectory(backend)

set(SOURCE_FILES main.cpp)
add_executable(test_tokenizer ${SOURCE_FILES})

target_link_libraries(test_tokenizer frontend)
target_link_libraries(test_tokenizer backend)