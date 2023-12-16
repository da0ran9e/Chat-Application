#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INTERNAL 1
#define LEAF 0
#define MAX 16	//MAXimum length of codeword, more the MAX more overhead
char padding;
unsigned char N;

typedef struct node
{
	char x;
	int freq;
	char *code;
	int type;
	struct node *next;
	struct node *left;
	struct node *right;
}node;

node *HEAD,*ROOT;

typedef struct symCode
{ char x;
  char code[MAX];
}symCode;

char ext[]=".hzip";
char dext[]=".txt";

void printll();
void makeTree();
void genCode(node *p,char* code);
void insert(node *p,node *m);
void addSymbol(char c);
void writeHeader(FILE *f);
void writeBit(int b,FILE *f);
void writeCode(char ch,FILE *f);
char *getCode(char ch);

#endif
