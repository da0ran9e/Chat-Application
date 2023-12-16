#include "huffman.h"
  
// allocate a new min heap node
struct MinHeapNode* newNode(char data, unsigned freq) { 
    struct MinHeapNode* temp = (struct MinHeapNode*)malloc(sizeof(struct MinHeapNode)); 
    temp->left = temp->right = NULL; 
    temp->data = data; 
    temp->freq = freq; 
  
    return temp; 
} 
  
// create a min heap of capacity 
struct MinHeap* createMinHeap(unsigned capacity) { 
    struct MinHeap* minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));  
    minHeap->size = 0; // current size  
    minHeap->capacity = capacity; 
    minHeap->array = (struct MinHeapNode**)malloc(minHeap->capacity * sizeof(struct MinHeapNode*)); 
    return minHeap; 
} 
  
// swap two nodes 
void swap(struct MinHeapNode** a, struct MinHeapNode** b) {
    struct MinHeapNode* t = *a; 
    *a = *b; 
    *b = t; 
} 
  
// heapify the min heap
void heapify(struct MinHeap* minHeap, int idx) { 
    int smallest = idx; 
    int left = 2 * idx + 1; 
    int right = 2 * idx + 2; 
  
    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq) smallest = left; 
    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq) smallest = right; 
    if (smallest != idx) { 
        swap(&minHeap->array[smallest], &minHeap->array[idx]); 
        heapify(minHeap, smallest); 
    } 
} 
  
// if size of heap is 1 
int isSizeOne(struct MinHeap* minHeap) { 
    return (minHeap->size == 1); 
} 
  
// get minimum node 
struct MinHeapNode* extractMin(struct MinHeap* minHeap) 
  
{ 
  
    struct MinHeapNode* temp = minHeap->array[0]; 
    minHeap->array[0] = minHeap->array[minHeap->size - 1]; 
  
    --minHeap->size; 
    heapify(minHeap, 0); 
  
    return temp; 
} 
  
//insert new node
void insertMinHeap(struct MinHeap* minHeap, struct MinHeapNode* minHeapNode) { 
  
    ++minHeap->size; 
    int i = minHeap->size - 1; 
  
    while (i 
           && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) { 
  
        minHeap->array[i] = minHeap->array[(i - 1) / 2]; 
        i = (i - 1) / 2; 
    } 
    minHeap->array[i] = minHeapNode; 
} 
  
// build min heap 
void buildMinHeap(struct MinHeap* minHeap) { 
  
    int n = minHeap->size - 1; 
    int i; 
    for (i = (n - 1) / 2; i >= 0; --i) 
        heapify(minHeap, i); 
} 
  
// print array
void printArr(int arr[], int n) 
{ 
    int i; 
    for (i = 0; i < n; ++i) 
        printf("%d", arr[i]); 
  
    printf("\n"); 
} 

int isLeaf(struct MinHeapNode* root) { 
  
    return !(root->left) && !(root->right); 
} 
  
// Initially heap
struct MinHeap* createAndBuildMinHeap(char data[], int freq[], int size) { 
  
    struct MinHeap* minHeap = createMinHeap(size); 
    for (int i = 0; i < size; ++i) minHeap->array[i] = newNode(data[i], freq[i]); 
    minHeap->size = size; 
    buildMinHeap(minHeap); 
  
    return minHeap; 
} 
  
// builds Huffman tree 
struct MinHeapNode* buildHuffmanTree(char data[], int freq[], int size) { 
    struct MinHeapNode *left, *right, *top; 
    // create a min heap 
    struct MinHeap* minHeap = createAndBuildMinHeap(data, freq, size); 

    while (!(minHeap->size == 1)) { //while size of heap !=1 
  
        // Extract the two minimum freq  
        left = extractMin(minHeap); 
        right = extractMin(minHeap); 
  
        //Create a new node with frequency equal to the sum of the two nodes. 
        // Make the two extracted node as left and right children

        top = newNode('$', left->freq + right->freq); 
  
        top->left = left; 
        top->right = right; 
  
        insertMinHeap(minHeap, top); 
    } 
  
    // the root is the remaining
    return extractMin(minHeap); 
} 
  
// Prints huffman codes from the root of Huffman Tree. 
void printCodes(struct MinHeapNode* root, int arr[], int top) { 

    if (root->left) { 
        arr[top] = 0; 
        printCodes(root->left, arr, top + 1); 
    } 
  
    if (root->right) { 
        arr[top] = 1; 
        printCodes(root->right, arr, top + 1); 
    } 
  
    // If this is a leaf node, then 
    // it contains one of the input 
    // characters, print the character 
    // and its code from arr[] 
    if (isLeaf(root)) { 
  
        printf("%c: ", root->data); 
        printArr(arr, top); 
    } 
} 
  
// The main function that builds a 
// Huffman Tree and print codes by traversing 
// the built Huffman Tree 
void HuffmanCodes(char data[], int freq[], int size) { 
    // Construct Huffman Tree 
    struct MinHeapNode* root = buildHuffmanTree(data, freq, size); 
    int arr[MAX_TREE_HT], top = 0; 
    printCodes(root, arr, top); 
} 

int readFromFile(const char *filename, char data[], int freq[]) {
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return 0; // Indicates failure
    }

    int i = 0; // for indexing
    char ch;    // character 
    int f[127]; // frequence of ascii
    int c = 0; // count of freq index
    int max = 0;

    for (int j=0; j<127; j++) f[j] = 0;

    while ((ch = fgetc(file)) != EOF) {
        f[ch]++;
        i++;
    }
    for (int j=0; j<127; j++) {
        if (j> 33 && j< 126){
            data[c] = j;
            freq[c] = f[j];
            c++;
        }
    }

    fclose(file);
    return i; // Indicates success
}

// Driver code 
int main() 
{ 
    char filename[] = "alice.txt"; // Change this to your file name
    int file_size;
    char file_data[105];
    int file_freq[105];

    file_size = readFromFile(filename, file_data, file_freq);
    // Call HuffmanCodes with data and frequencies from the file
    HuffmanCodes(file_data, file_freq, 105);


    return 0;
}