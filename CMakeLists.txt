cmake_minimum_required(VERSION 3.27)
project(parallel_assignment_2)

set(CMAKE_CXX_STANDARD 14)

# Add MPI Package to Project
find_package(MPI REQUIRED)

add_executable(parallel_assignment_2 parallel-assignment-2.cpp)

# Add libraries for code completion and compiling
target_link_libraries(parallel_assignment_2 PUBLIC MPI::MPI_CXX)
