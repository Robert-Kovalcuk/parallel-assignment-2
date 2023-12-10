//
// Created by bobok on 10/12/2023.
//
#include <stdint.h>

#ifndef PARALLEL_ASSIGNMENT_2_FILELOADUTILS_H
#define PARALLEL_ASSIGNMENT_2_FILELOADUTILS_H

#define WIDTH 1024
#define HEIGHT 1024
#define DEPTH 314

// Function to read voxel data from a RAW file
uint8_t* readVoxelData(const char* filename);

// Function to free allocated memory for voxel data
void freeVoxelData(uint8_t* voxels);

// Function to access voxel value at coordinates (x, y, z)
uint8_t getVoxelValue(const uint8_t* voxels, int x, int y, int z);

#endif //PARALLEL_ASSIGNMENT_2_FILELOADUTILS_H
