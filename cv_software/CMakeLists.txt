cmake_minimum_required(VERSION 3.10)
project(bloom_filter)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/filter.cpp)
