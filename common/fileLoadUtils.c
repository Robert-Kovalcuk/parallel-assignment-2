//
// Created by bobok on 10/12/2023.
//
#include "fileLoadUtils.h"
#include <stdio.h>
#include <stdlib.h>

uint8_t* readVoxelData(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Unable to open file: %s\n", filename);
        exit(1);
    }

    // Calculate the size of the volume
    size_t volumeSize = X * Y * Z;

    // Allocate memory to store the voxel data
    uint8_t* voxels = (uint8_t*)malloc(volumeSize * sizeof(uint8_t));
    if (!voxels) {
        fclose(file);
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }

    // Read the voxel data from the RAW file
    size_t bytesRead = fread(voxels, sizeof(uint8_t), volumeSize, file);
    fclose(file);

    if (bytesRead != volumeSize) {
        fprintf(stderr, "Error reading voxel data.\n");
        freeVoxelData(voxels);
        exit(1);
    }

    return voxels;
}

void freeVoxelData(uint8_t* voxels) {
    free(voxels);
}

uint8_t getVoxelValue(const uint8_t* voxels, int x, int y, int z) {
    // Validate coordinates
    if (x < 0 || x >= X || y < 0 || y >= Y || z < 0 || z >= Z) {
        fprintf(stderr, "Invalid voxel coordinates.\n");
        exit(1);
    }

    // Calculate the linear index for the given coordinates
    size_t index = x + X * (y + Y * z);

    return voxels[index];
}