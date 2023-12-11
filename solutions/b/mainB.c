#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/time.h>
#include "C:\Users\bobok\CLionProjects\parallel-assignment-2.v2\common\fileLoadUtils.h"

#define THRESHOLD 25
#define NUM_THREADS 3

struct ThreadData {
    uint8_t *voxels;
    int start;
    int end;
};

void *processSubset(void *arg) {
    struct ThreadData *data = (struct ThreadData *) arg;
    int byteCount = 1;
    bool flip = 0;

    for (int j = data->start; j < data->end; j++) {
        if (data->voxels[j] > THRESHOLD && !flip) {
            byteCount++;
            flip = 1;
        }

        if (data->voxels[j] <= THRESHOLD && flip) {
            byteCount++;
            flip = 0;
        }
    }

    return (void *) (intptr_t) byteCount;
}

int main() {
    uint8_t *voxels = readVoxelData("C:\\Users\\bobok\\CLionProjects\\parallel-assignment-2.v2\\c8.raw");
    int len = X * Y * Z;

    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    int totalByteCount = 1;
    pthread_t threads[NUM_THREADS];
    struct ThreadData threadData[NUM_THREADS];

    int chunkSize = len / NUM_THREADS;
    int remainder = len % NUM_THREADS;

    int i;
    for (i = 0; i < NUM_THREADS; i++) {
        threadData[i].voxels = voxels;
        threadData[i].start = i * chunkSize;
        threadData[i].end = (i == NUM_THREADS - 1) ? (i + 1) * chunkSize + remainder : (i + 1) * chunkSize;

        pthread_create(&threads[i], NULL, processSubset, (void *) &threadData[i]);
    }

    for (i = 0; i < NUM_THREADS; i++) {
        void *result;
        pthread_join(threads[i], &result);
        totalByteCount += (int) (intptr_t) result;
    }

    gettimeofday(&end_time, NULL);

    printf("Byte: %d\n", totalByteCount);

    long elapsed = (end_time.tv_sec - start_time.tv_sec) * 1000000 + end_time.tv_usec - start_time.tv_usec;
    printf("Time: %ld ms\n", elapsed / 1000);

    free(voxels);

    return 0;
}
