#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFSIZE 256
int NUM=0;

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

int FileRead(const char *file, char **buff);
void enCode(HTNodep T, char *s, Code *Codep, int *NUM);
char *printStruct(HTNodep T, char *s);
int Frequence_Count(char buff[], int *freq, char *lett);
bool insert(minHeap H, HTNodep hTree);
bool IsFull(minHeap H);
bool IsEmpty(minHeap H);
char *MatchingString(const char *s1, const char *s2);
void Translate(char str[], HTNodep T, FILE *fp);
HTNodep buildTree(int max, int freq[], char lett[]);
HTNodep DeleteMin(minHeap H);
HTNodep NewHuffmanNode();
minHeap CreateMinHeap(int MaxSize);
minHeap buildMinHeap(minHeap H);

int FileRead(const char *file, char **buff) {
    FILE *fp = fopen(file, "r");
    if (fp == NULL) {
        printf("file doesn't exist\n");
        return 0;
    }

    fseek(fp, 0, SEEK_END);
    long fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    *buff = (char *)malloc((fileSize + 1) * sizeof(char));
    if (*buff == NULL) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    size_t readSize = fread(*buff, sizeof(char), fileSize, fp);
    if (readSize != (size_t)fileSize) {
        perror("Reading error");
        exit(EXIT_FAILURE);
    }

    (*buff)[fileSize] = '\0';
    
    if (fclose(fp) == EOF) {
        perror("Error closing file");
        exit(EXIT_FAILURE);
    }

    return 1;
}

char *MatchingString(const char *s1, const char *s2) {
    char *t = (char *)malloc(strlen(s1) + strlen(s2) + 1);
    if (t == NULL) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    strcpy(t, s1);
    strcat(t, s2);

    return t;
}

void enCode(HTNodep T, char *s, Code *Codep, int *NUM) {
    if (T->leftChild)
        enCode(T->leftChild, MatchingString(s, "0"), Codep, NUM);
    else {
        Code temp = {T->letter, (char*)malloc(strlen(s) + 1)};  // Use malloc to allocate memory
        if (temp.s == NULL) {
            perror("Memory allocation error");
            exit(EXIT_FAILURE);
        }
        strcpy(temp.s, s);
        Codep[*NUM] = temp;
        (*NUM)++;
        free(temp.s);
        return;
    }

    if (T->rightChild)
        enCode(T->rightChild, MatchingString(s, "1"), Codep, NUM);
}

char *printStruct(HTNodep T, char *s) {
    char str[1000];
    while (T) {
        sprintf(str, "%d", T->order);
		s = MatchingString(s, str);

        s = MatchingString(s, &(T->letter));
        sprintf(str, "%d", T->weight);
		s = MatchingString(s, str);

        s = printStruct(T->leftChild, s);
        s = printStruct(T->rightChild, s);
    }
    return s;
}

bool IsFull(minHeap H) {
    return (H->size == H->maxSize);
}

bool IsEmpty(minHeap H) {
    return (H->size == 0);
}

minHeap CreateMinHeap(int MaxSize) {
    minHeap H = (minHeap)malloc(sizeof(Heap));
    H->HT = (HTNodep *)malloc((MaxSize + 1) * sizeof(HTNodep));
    H->size = 0;
    H->maxSize = MaxSize;
    HTNodep T = NewHuffmanNode();
    T->weight = -1;
    T->letter = '0';
    H->HT[0] = T;

    return H;
}

HTNodep NewHuffmanNode() {
    HTNodep BST = (HTNodep)malloc(sizeof(HTNode));
    BST->weight = 0;
    BST->letter = '0';
    BST->order = 0;
    BST->leftChild = BST->rightChild = NULL;

    return BST;
}

minHeap buildMinHeap(minHeap H) {
    int i, parent, child;
    HTNodep temp;
    for (i = H->size / 2; i > 0; i--) {
        temp = H->HT[i];
        for (parent = i; parent * 2 <= H->size; parent = child) {
            child = parent * 2;
            if ((child != H->size) && (H->HT[child]->weight > H->HT[child + 1]->weight)) {
                child++;
            }
            if (temp->weight > H->HT[child]->weight) {
                H->HT[parent] = H->HT[child];
            } else {
                break;
            }
        }
        H->HT[parent] = temp;
    }
    return H;
}

HTNodep DeleteMin(minHeap H) {
    int parent, child;
    HTNodep MinItem, temp = NULL;

    if (IsEmpty(H)) {
        printf("最小堆为空\n");
        return NULL;
    }

    MinItem = H->HT[1];
    temp = H->HT[H->size--];

    for (parent = 1; parent * 2 <= H->size; parent = child) {
        child = parent * 2;
        if ((child != H->size) && (H->HT[child]->weight > H->HT[child + 1]->weight)) {
            child++;
        }
        if (temp->weight > H->HT[child]->weight) {
            H->HT[parent] = H->HT[child];
        } else {
            break;
        }
    }

    H->HT[parent] = temp;

    return MinItem;
}

bool insert(minHeap H, HTNodep hTree) {
    int i;
    if (IsFull(H)) {
        printf("最小堆已满\n");
        return false;
    }
    i = ++H->size;
    for (; H->HT[i / 2]->weight > hTree->weight; i /= 2)
        H->HT[i] = H->HT[i / 2];
    H->HT[i] = hTree;

    return true;
}

HTNodep buildTree(int max, int freq[], char lett[]) {
    minHeap H = CreateMinHeap(max);
    HTNodep T;
    for (int i = 1; i <= H->maxSize / 2; i++) {
        T = NewHuffmanNode();
        T->weight = freq[i - 1];
        T->letter = lett[i - 1];
        H->HT[++(H->size)] = T;
    }

    buildMinHeap(H);

    int i, num;
    num = H->size;
    for (i = 1; i < num; i++) {
        T = NewHuffmanNode();
        T->leftChild = DeleteMin(H);
        T->rightChild = DeleteMin(H);
        T->weight = T->leftChild->weight + T->rightChild->weight;
        T->order = i - 1;
        insert(H, T);
    }

    T = DeleteMin(H);

    return T;
}

int Frequence_Count(char buff[], int *freq, char *lett) {
    char *beg = buff;
    int cnt[256] = {0};
    while (*beg) {
        ++cnt[*beg];
        ++beg;
    }

    int ret = 0;
    for (int i = 0; i < 256; ++i) {
        if (cnt[i] != 0) {
            lett[ret] = (char)i;
            freq[ret] = cnt[i];
            ++ret;
        }
    }

    return ret;
}

void Translate(char str[], HTNodep T, FILE *fp) {
    int i = 0, cnt = 0;
    HTNodep flag = T;
    while (str[i]) {
        if (cnt == NUM)
            break;
        if (str[i] == '0')
            flag = flag->leftChild;
        else if (str[i] == '1')
            flag = flag->rightChild;
        if (flag->leftChild == NULL)
            fputc(flag->letter, fp);
        flag = T;
        cnt++;
        i++;
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s inputfile outputfile\n", argv[0]);
        return 1;
    }

    char *fileInfoRead = NULL;
    if (!FileRead(argv[1], &fileInfoRead)) {
        return 1;
    }

    int Freq[256];
    char b[256];
    int setSize = Frequence_Count(fileInfoRead, Freq, b);
    HTNodep T = buildTree(setSize * 2, Freq, b);

    FILE *outputFile = fopen(argv[2], "w");
    if (outputFile == NULL) {
        perror("Error opening output file");
        return 1;
    }

    char strc[1000], code[1000];
    Code *Codep = (Code *)malloc(setSize * sizeof(Code));
    if (Codep == NULL) {
        perror("Memory allocation error");
        return 1;
    }

    printStruct(T, strc);
    enCode(T, code, Codep, &NUM);
    strcat(strc, "EndHuffmanTree");
    strcat(strc, code);

    // You can use the 'outputFile' file pointer to write the 'strc' and other data to the output file
    // Example:
    fprintf(outputFile, "%s", strc);

    // Close the output file
    if (fclose(outputFile) == EOF) {
        perror("Error closing output file");
        return 1;
    }

    // Free allocated memory
    free(fileInfoRead);
    free(Codep);

    return 0;
}
