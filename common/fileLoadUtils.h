//
// Created by bobok on 10/12/2023.
//
#include <stdint.h>

#ifndef PARALLEL_ASSIGNMENT_2_FILELOADUTILS_H
#define PARALLEL_ASSIGNMENT_2_FILELOADUTILS_H

#define X 1024
#define Y 1024
#define Z 314

uint8_t* readVoxelData(const char* filename);
void freeVoxelData(uint8_t* voxels);
uint8_t getVoxelValue(const uint8_t* voxels, int x, int y, int z);

#endif //PARALLEL_ASSIGNMENT_2_FILELOADUTILS_H
