cmake_minimum_required(VERSION 3.26.4)
project(parallel_assignment_2)

set(CMAKE_CXX_STANDARD 14)

# Add MPI Package to Project
# find_package(MPI REQUIRED)

add_executable(parallel_assignment_2 main.c
        common/fileLoadUtils.h
        common/fileLoadUtils.c
        )

add_executable(mainC
        common/fileLoadUtils.h
        common/fileLoadUtils.c
        solutions/c/mainC.c)

add_executable(mainB
        common/fileLoadUtils.h
        common/fileLoadUtils.c
        solutions/b/mainB.c)

# Add libraries for code completion and compiling
#target_link_libraries(parallel_assignment_2 PUBLIC MPI::MPI_CXX)
