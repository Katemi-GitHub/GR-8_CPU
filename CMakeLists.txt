cmake_minimum_required(VERSION 3.20)

set(PROJECT_NAME "gr8")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -Wextra -g -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function")

set(INCLUDE "include/")

set(SOURCES
    "src/main.cpp"
)

find_package(SDL2 CONFIG REQUIRED)

project(${PROJECT_NAME} C CXX)
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${SDL2_INCLUDE_DIRS}
    ${INCLUDE}
)