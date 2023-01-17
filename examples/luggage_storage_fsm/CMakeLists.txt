cmake_minimum_required(VERSION 3.14)

project(luggage_storage_fsm LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../include)

add_executable(luggage_storage_fsm main.cpp)
