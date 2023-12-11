#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "../../common/fileLoadUtils.h"
#include <stdbool.h>

#define THRESHOLD 25
#define NUM_THREADS 4


int main() {
    uint8_t* voxels = readVoxelData("/home/eddie/TUKE/PP/parallel-assignment-2/c8.raw");;
    int len = WIDTH * HEIGHT * DEPTH;

    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    bool flip = 0;
    int totalByteCount = 1;

    if (voxels[0] <= THRESHOLD) {
        flip = 0;
    }

    for (int i = 0; i < len/1024; ++i) {
        int byteCount = 1;

        for (int j = i*1024; j < (i*1024)+1024; j++) {

            if (voxels[j] > THRESHOLD && !flip) {
                byteCount++;
                flip = 1;
            }

            if (voxels[j] <= THRESHOLD && flip) {
                byteCount++;
                flip = 0;
            }
        }

        totalByteCount += byteCount;
    }

    gettimeofday(&end_time, NULL);

    printf("Total byte count: %d\n", totalByteCount);

    long elapsed = (end_time.tv_sec - start_time.tv_sec) * 1000000 + end_time.tv_usec - start_time.tv_usec;
    printf("Time elapsed: %ld ms\n", elapsed / 1000);

    free(voxels);

    return 0;
}
