#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h> 
#include <stdlib.h> 
  
#define MAX_TREE_HT 105 
 
// A Huffman tree node 
struct MinHeapNode { 
    char data; 
    unsigned freq; 
    struct MinHeapNode *left, *right; 
}; 
  
//min-heap
struct MinHeap { 
    unsigned size;  
    unsigned capacity; 
    struct MinHeapNode** array; 
}; 

int 		readFromFile		(const char *filename, char data[], int freq[]);
void 		swapMinHeapNode		(struct MinHeapNode** a, struct MinHeapNode** b);
void 		minHeapify			(struct MinHeap* minHeap, int idx); 
int 		isSizeOne			(struct MinHeap* minHeap);
struct MinHeapNode* extractMin	(struct MinHeap* minHeap);
void 		insertMinHeap		(struct MinHeap* minHeap, struct MinHeapNode* minHeapNode);
void 		buildMinHeap		(struct MinHeap* minHeap);
void 		printArr			(int arr[], int n);
int 		isLeaf				(struct MinHeapNode* root);
struct MinHeap* createAndBuildMinHeap(char data[], int freq[], int size);
struct MinHeapNode* buildHuffmanTree(char data[], int freq[], int size);
void 		printCodes			(struct MinHeapNode* root, int arr[], int top); 
void 		HuffmanCodes		(char data[], int freq[], int size);


#endif