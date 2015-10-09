cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(
${PROJECT_SOURCE_DIR}
)

aux_source_directory(. DIR_SRCS)

add_library(backend ${DIR_SRCS})
# target_link_libraries(backend frontend common)