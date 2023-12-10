#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include "../../common/fileLoadUtils.h"

#define THRESHOLD 25
#define BLOCK_SIZE 1024
#define NUM_THREADS 4

typedef struct {
    unsigned char value;
} Voxel;

typedef struct {
    unsigned char *metadata;
} MetadataBlock;

typedef struct {
    int start;
    int end;
    Voxel ***data;
    MetadataBlock *metadataBlock;
} ThreadData;

void applyThresholdPartial(Voxel ***data, MetadataBlock *metadata, int start, int end) {
    for (int z = 0; z < DEPTH; z++) {
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = start; x < end; x++) {
                if (data[x][y][z].value > THRESHOLD) {
                    metadata->metadata[x + y * WIDTH + z * WIDTH * HEIGHT] = 1;
                } else {
                    metadata->metadata[x + y * WIDTH + z * WIDTH * HEIGHT] = 0;
                }
            }
        }
    }
}

void *threadFunction(void *arg) {
    ThreadData *threadData = (ThreadData *)arg;
    applyThresholdPartial(threadData->data, threadData->metadataBlock, threadData->start, threadData->end);
    pthread_exit(NULL);
    return NULL;
}

int calculateCompressedSize(const unsigned char *metadata, int start, int end) {
    int compressedSize = 0;
    int count = 1;

    for (int i = start + 1; i < end; i++) {
        if (metadata[i] == metadata[i - 1]) {
            count++;
        } else {
            compressedSize += 2;
            count = 1;
        }
    }

    compressedSize += 2;

    return compressedSize;
}

int main() {
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    FILE *file = fopen("c8.raw", "rb");
    if (file == NULL) {
        fprintf(stderr, "Error opening file.\n");
        return 1;
    }

    Voxel ***data = (Voxel ***)malloc(WIDTH * sizeof(Voxel **));
    for (int x = 0; x < WIDTH; x++) {
        data[x] = (Voxel **)malloc(HEIGHT * sizeof(Voxel *));
        for (int y = 0; y < HEIGHT; y++) {
            data[x][y] = (Voxel *)malloc(DEPTH * sizeof(Voxel));
        }
    }

    fread(data[0][0], sizeof(Voxel), WIDTH * HEIGHT * DEPTH, file);

    fclose(file);

    MetadataBlock metadataBlock;
    metadataBlock.metadata = (unsigned char *)malloc(WIDTH * HEIGHT * DEPTH * sizeof(unsigned char));
    memset(metadataBlock.metadata, 0, WIDTH * HEIGHT * DEPTH);

    pthread_t threads[NUM_THREADS];
    ThreadData threadData[NUM_THREADS];

    int blockSize = WIDTH / NUM_THREADS;
    for (int i = 0; i < NUM_THREADS; i++) {
        threadData[i].start = i * blockSize;
        threadData[i].end = (i == NUM_THREADS - 1) ? WIDTH : (i + 1) * blockSize;
        threadData[i].data = data;
        threadData[i].metadataBlock = &metadataBlock;

        pthread_create(&threads[i], NULL, threadFunction, (void *)&threadData[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    int totalCompressedSize = 0;
    for (int i = 0; i < WIDTH; i += BLOCK_SIZE) {
        int blockEnd = (i + BLOCK_SIZE > WIDTH) ? WIDTH : i + BLOCK_SIZE;
        int compressedSize = calculateCompressedSize(metadataBlock.metadata, i, blockEnd);
        totalCompressedSize += compressedSize;
    }

    printf("Total compressed size: %d bytes\n", totalCompressedSize);

    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            free(data[x][y]);
        }
        free(data[x]);
    }
    free(data);
    free(metadataBlock.metadata);

    gettimeofday(&end_time, NULL);
    long elapsed = (end_time.tv_sec - start_time.tv_sec) * 1000000 + end_time.tv_usec - start_time.tv_usec;
    printf("Time elapsed: %ld microseconds\n", elapsed);

    return 0;
}
