cmake_minimum_required(VERSION 3.26.4)
project(parallel_assignment_2)

set(CMAKE_CXX_STANDARD 14)

# Add MPI Package to Project
# find_package(MPI REQUIRED)

# Common source files
set(COMMON_FILES
        common/fileLoadUtils.h
        common/fileLoadUtils.c
)

# Target for mainB
add_executable(mainB
        solutions/b/mainB.c
        ${COMMON_FILES}
)

# Target for mainC
add_executable(mainC
        solutions/c/mainC.c
        ${COMMON_FILES}
)

# Target for mainA
add_executable(mainA
        solutions/a/mainA.c
        ${COMMON_FILES}
)

# Add libraries for code completion and compiling
# target_link_libraries(mainB PUBLIC MPI::MPI_CXX)
