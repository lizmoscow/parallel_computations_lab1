cmake_minimum_required(VERSION 3.19)
project(ParallelComputations)

set(CMAKE_C_COMPILER /opt/homebrew/Cellar/gcc/11.2.0/bin/gcc-11)
set(CMAKE_CPP_COMPILER /opt/homebrew/Cellar/gcc/11.2.0/bin/g++-11)
set(CMAKE_CXX_STANDARD 20)
add_executable(ParallelComputations lab_parallel.cpp)