#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>

#define X_DIM 1024
#define Y_DIM 1024
#define Z_DIM 314
#define THRESHOLD 25
#define BLOCK_SIZE 1024
#define NUM_THREADS 16

uint8_t filedata[X_DIM * Y_DIM * Z_DIM];
pthread_t threads[NUM_THREADS];
pthread_mutex_t totalCompressedSizeLock = PTHREAD_MUTEX_INITIALIZER;
int totalCompressedSize = 0;

// Štruktúra pre predávanie argumentov vláknu
struct ThreadArgs {
    int start;
    int end;
};

// Funkcia na kompresiu bit-level Run-Length Encoding
int compressRLE(const uint8_t *data, int dataSize, uint8_t *compressedData) {
    int compressedSize = 0;
    int count = 1;

    for (int i = 1; i < dataSize; i++) {
        if (data[i] == data[i - 1]) {
            count++;
        } else {
            // Zapisuje sa hodnota bitu a počet opakovaní do 8 bitov
            compressedData[compressedSize++] = (data[i - 1] << 7) | (count & 0x7F);
            count = 1;
        }
    }

    // Spracovanie poslednej sekvencie
    compressedData[compressedSize++] = (data[dataSize - 1] << 7) | (count & 0x7F);

    return compressedSize;
}

// Funkcia vykonávaná v každom vlákne
void *processBlock(void *threadArgs) {
    struct ThreadArgs *args = (struct ThreadArgs *)threadArgs;
    int start = args->start;
    int end = args->end;

    for (int x = start; x < end; x += BLOCK_SIZE) {

        // Skopírovanie metadát do pomocného poľa
        uint8_t compressedMetadata[BLOCK_SIZE];
        for (int i = 0; i < BLOCK_SIZE; i++) {
            compressedMetadata[i] = filedata[x];
        }

        // Kompresia metadát pomocou bit-level RLE
        int compressedSize = compressRLE(compressedMetadata, BLOCK_SIZE, compressedMetadata);

        // Aktualizácia celkovej veľkosti komprimovaných dát
        pthread_mutex_lock(&totalCompressedSizeLock);
        totalCompressedSize += compressedSize;
        pthread_mutex_unlock(&totalCompressedSizeLock);
    }

    pthread_exit(NULL);

    return NULL;
}

int main() {
    FILE *file;
    char *filename = "c8.raw";

    // Otvorenie súboru na čítanie binárnych dát
    file = fopen(filename, "rb");
    if (file == NULL) {
        fprintf(stderr, "Nepodarilo sa otvoriť súbor %s\n", filename);
        exit(1);
    }

    // Načítanie dát zo súboru
    for (int x = 0; x < X_DIM * Y_DIM * Z_DIM; x++) {
        uint8_t tmp;
        fread(&tmp, sizeof(uint8_t), 1, file);
        // Aplikácia prahovania
        filedata[x] = (tmp > THRESHOLD) ? 1 : 0;
    }

    // Zatvorenie súboru
    fclose(file);

    // Inicializácia a spustenie vlákien
    struct ThreadArgs threadArgs[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        threadArgs[i].start = i * X_DIM * Y_DIM * Z_DIM / BLOCK_SIZE;
        threadArgs[i].end = (i + 1) * X_DIM * Y_DIM * Z_DIM / BLOCK_SIZE;
        pthread_create(&threads[i], NULL, processBlock, (void *)&threadArgs[i]);
    }

    // Čakanie na ukončenie všetkých vlákien
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Celkova velkost komprimovanych dat pre vsetky datove bloky je %d bajtov\n", totalCompressedSize);

    return 0;
}
