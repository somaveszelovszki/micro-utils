cmake_minimum_required(VERSION 3.10)
project(micro_utils)

include_directories(
    "src"
    "../include"
)

file(GLOB SOURCES
    "../src/*.c"
    "../src/*.cpp"
    "src/*.cpp"
)

add_executable(${PROJECT_NAME}_test ${SOURCES})

add_test(NAME ${PROJECT_NAME}_test COMMAND ${PROJECT_NAME}_test)

target_link_libraries(${PROJECT_NAME}_test PUBLIC gtest)
