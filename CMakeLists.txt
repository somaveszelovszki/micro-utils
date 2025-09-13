cmake_minimum_required(VERSION 3.25)
project(micro_utils)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wno-template-in-declaration-name)

add_subdirectory(thirdparty/etl)

file(GLOB SOURCES
    "src/*.c"
    "src/*.cpp"
    "thirdparty/tiny-json/tiny-json.c"
)

add_library(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include thirdparty/tiny-json)

target_link_libraries(${PROJECT_NAME} PUBLIC etl::etl)

if (BUILD_TESTS)
    message("Tests are enabled")
    enable_testing()
    add_subdirectory(thirdparty/googletest)
    add_subdirectory(test)
endif()

if (BUILD_BENCHMARKS)
    message("Benchmarks are enabled")
    add_subdirectory(thirdparty/benchmark)
    add_subdirectory(benchmark)
endif()
