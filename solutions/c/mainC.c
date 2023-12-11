#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "../../common/fileLoadUtils.h"
#include <stdbool.h>
#include <mpi.h>

#define THRESHOLD 25
#define NUM_THREADS 4


int main() {
    MPI_Init(NULL, NULL);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != NUM_THREADS) {
        if (rank == 0) {
            fprintf(stderr, "This program requires %d MPI processes.\n", NUM_THREADS);
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    uint8_t* voxels = NULL;
    int len;

    if (rank == 0) {
        voxels = readVoxelData("/home/eddie/TUKE/PP/parallel-assignment-2/c8.raw");
        len = WIDTH * HEIGHT * DEPTH;
    }

    MPI_Bcast(&len, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int start = rank * len / size;
    int end = (rank + 1) * len / size;

    uint8_t* subsetVoxels = (uint8_t*)malloc((end - start) * sizeof(uint8_t));

    MPI_Scatter(voxels, end - start, MPI_UNSIGNED_CHAR, subsetVoxels, end - start, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    bool flip = 0;
    int byteCount = 1;

    if (subsetVoxels[0] <= THRESHOLD) {
        flip = 0;
    }

    for (int i = 0; i < end - start; i++) {
        if (subsetVoxels[i] > THRESHOLD && !flip) {
            byteCount++;
            flip = 1;
        }

        if (subsetVoxels[i] <= THRESHOLD && flip) {
            byteCount++;
            flip = 0;
        }
    }

    printf("Rank: %d, Byte count: %d\n", rank, byteCount);

    int globalByteCount = 0;

    MPI_Reduce(&byteCount, &globalByteCount, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    gettimeofday(&end_time, NULL);

    if (rank == 0) {
        printf("\nTotal byte count: %d\n", globalByteCount);

        long elapsed = (end_time.tv_sec - start_time.tv_sec) * 1000000 + end_time.tv_usec - start_time.tv_usec;
        printf("Time elapsed: %ld ms\n", elapsed / 1000);
    }

    free(subsetVoxels);

    MPI_Finalize();

    return 0;
}
