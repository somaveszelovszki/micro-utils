cmake_minimum_required(VERSION 3.10)
project(micro_utils)

add_subdirectory(thirdparty/ArduinoJson)
add_subdirectory(thirdparty/etl)

file(GLOB SOURCES
    "src/*.c"
    "src/*.cpp"
)

add_library(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} PUBLIC ArduinoJson etl::etl)

if (BUILD_TESTS)
    message("Tests are enabled")
    enable_testing()
    add_subdirectory(thirdparty/googletest)
    add_subdirectory(test)
endif()
