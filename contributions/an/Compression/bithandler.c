#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define BUFFSIZE 256
#define CODE 10000007

int NUM = 0;

typedef struct huffmanTreeNode {
    char letter;
    int weight;
    struct huffmanTreeNode* leftChild;
    struct huffmanTreeNode* rightChild;
    int order;
} HTNode, *HTNodep;

typedef struct {
    HTNodep* HT;
    int size;
    int maxSize;
} Heap, *minHeap;

typedef struct {
    char ch;
    char *s;
} Code;

void writeBinaryFile(int length, const char *buf, const char *filePath);
char *readBinaryFile(const char *filePath, int *memory);
void encode_to_bit(const char *str, char *bit);
void decode_to_str(int strLen, const char *bit, char *str);
void saveFile(const char *filePath, const char *huffmanTree, const char *fileBit);
void loadFile(const char *filePath, int *hufLength, int *bitLen, char **huffmanTree, char **fileBit);

void writeBinaryFile(int length, const char *buf, const char *filePath) {
    FILE *pFile = fopen(filePath, "wb");
    if (pFile == NULL) {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }
    fwrite(buf, sizeof(char), length, pFile);
    fclose(pFile);
}

char *readBinaryFile(const char *filePath, int *memory) {
    FILE *pFile = fopen(filePath, "rb");
    if (pFile == NULL) {
        perror("Error opening file for reading");
        exit(EXIT_FAILURE);
    }

    fseek(pFile, 0, SEEK_END);
    long lSize = ftell(pFile);
    rewind(pFile);

    char *buffer = (char *)malloc(sizeof(char) * lSize);
    if (buffer == NULL) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    size_t result = fread(buffer, sizeof(char), lSize, pFile);
    if (result != (size_t)lSize) {
        perror("Reading error");
        exit(EXIT_FAILURE);
    }

    *memory = (int)lSize;
    fclose(pFile);

    return buffer;
}

void encode_to_bit(const char *str, char *bit) {
    int strLen = (int)strlen(str);
    int bitLen = (strLen + 7) / 8;
    memset(bit, 0, sizeof(char) * bitLen);

    for (int i = 0; i < strLen; ++i) {
        unsigned int mask = i % 8;
        bit[i / 8] |= (char)((str[i] - '0') << mask);
    }
}

void decode_to_str(int strLen, const char *bit, char *str) {
    for (int i = 0; i < strLen; ++i) {
        unsigned int mask = 1 << (i % 8);
        str[i] = ((bit[i / 8] & mask) != 0) + '0';
    }
}

void saveFile(const char *filePath, const char *huffmanTree, const char *fileBit) {
    int length = (int)strlen(fileBit);
    int hufLength = (int)strlen(huffmanTree);
    int totMem = sizeof(char) * (hufLength + (length + 7) / 8 + sizeof(int) / sizeof(char) * 3);

    char *memSaved = (char *)malloc(totMem);
    if (memSaved == NULL) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    char *bitsaved = memSaved + sizeof(char) * (hufLength + sizeof(int) / sizeof(char) * 3);

    int *p = (int *)memSaved;
    *p = CODE;
    *(p + 1) = hufLength;
    *(p + 2) = length;

    memcpy(p + 3, huffmanTree, sizeof(char) * hufLength);
    encode_to_bit(fileBit, bitsaved);

    writeBinaryFile(totMem, memSaved, filePath);
    free(memSaved);
}

void loadFile(const char *filePath, int *hufLength, int *bitLen, char **huffmanTree, char **fileBit) {
    int totMem;
    char *memSaved = readBinaryFile(filePath, &totMem);
    int *code = (int *)memSaved;

    if (*code != CODE) {
        fprintf(stderr, "File type error!\n");
        *hufLength = *bitLen = 0;
        *huffmanTree = NULL;
        *fileBit = NULL;
        free(memSaved);
        return;
    }

    *hufLength = *(code + 1);
    *bitLen = *(code + 2);

    char *bitsaved = memSaved + sizeof(char) * (*hufLength + sizeof(int) / sizeof(char) * 3);
    *huffmanTree = (char *)malloc(sizeof(char) * (*hufLength + 1));

    if (*huffmanTree == NULL) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    (*huffmanTree)[*hufLength] = '\0';
    memcpy(*huffmanTree, (code + 3), sizeof(char) * (*hufLength));

    *fileBit = (char *)malloc(sizeof(char) * (*bitLen + 1));

    if (*fileBit == NULL) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    (*fileBit)[*bitLen] = '\0';
    decode_to_str(*bitLen, bitsaved, *fileBit);

    free(memSaved);
}

int main() {
    saveFile("temp", "ss", "000010111011");

    char *huf, *bit;
    int lh, lb;
    loadFile("temp", &lh, &lb, &huf, &bit);
    puts(bit);

    // Free allocated memory
    free(huf);
    free(bit);

    return 0;
}
