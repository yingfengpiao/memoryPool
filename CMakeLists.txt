cmake_minimum_required(VERSION 3.10)
project(MemoryPool)

include_directories(include)

add_executable(test test/test.cpp src/MemoryPool.cpp)