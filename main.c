#include "common/fileLoadUtils.h"
#include <stdio.h>

int main(int argc, char** argv) {
    const char* filename = "../c8.raw";

    uint8_t* voxels = readVoxelData(filename);

    // Example: Access voxel value at coordinates (10, 20, 30)
    int x = 10, y = 20, z = 30;
    uint8_t voxelValue = getVoxelValue(voxels, x, y, z);

    printf("Voxel value at coordinates (%d, %d, %d): %u\n", x, y, z, voxelValue);


    freeVoxelData(voxels);

    return 0;
}
