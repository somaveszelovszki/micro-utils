cmake_minimum_required(VERSION 3.10)
project(micro_utils)

add_subdirectory(gtest)

include_directories(
    "src"
    "../include"
)

file(GLOB SOURCES
    "../src/*.c"
    "../src/*.cpp"
    "src/*.cpp"
)

add_executable(${CMAKE_PROJECT_NAME}_test ${SOURCES})

add_test(NAME ${CMAKE_PROJECT_NAME}_test COMMAND ${CMAKE_PROJECT_NAME}_test)

target_link_libraries(${CMAKE_PROJECT_NAME}_test PUBLIC gtest)
