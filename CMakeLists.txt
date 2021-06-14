# cmake_minimum_required(VERSION <specify CMake version here>)
project(lab3 C)
cmake_minimum_required(VERSION 3.19)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

set(CURL_LIBRARY "-lcurl")
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

add_executable(lab3 main.c Server/server.c Server/server.h Client/client.c Client/client.h Server/config_worker.c Server/config_worker.h)
target_link_libraries(lab3 ${CURL_LIBRARIES})
