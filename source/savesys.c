#include <stdio.h>
#include "savesys.h"

// Save coins to file
void saveCoins(int coinCount) {
    FILE *file = fopen(SAVE_FILE, "wb");
    if (file) {
        fwrite(&coinCount, sizeof(int), 1, file);
        fclose(file);
    }
}

// Load coins from file
int loadCoins() {
    int coinCount = 0;
    FILE *file = fopen(SAVE_FILE, "rb");
    if (file) {
        fread(&coinCount, sizeof(int), 1, file);
        fclose(file);
    }
    return coinCount;
}