cmake_minimum_required(VERSION 3.14)

project(pimpl_idiom_fsm LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../include)

add_executable(pimpl_idiom_fsm main.cpp pimpl.h pimpl.cpp)
