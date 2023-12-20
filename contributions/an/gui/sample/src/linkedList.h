#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "type.h"


typedef struct Node {
    account_t* acc;
    struct Node* next;
} accNode_t;

typedef struct MsgNode {
    chat_msg_t* msg;
    struct MsgNode* next;
} chat_msg_node_t;

accNode_t* createNode (account_t* acc);

accNode_t* createLinkedList ();

accNode_t* insertNode (accNode_t* head, accNode_t* inode);

accNode_t* insertNodeHead (accNode_t* head, accNode_t* inode);

accNode_t* searchAccount (accNode_t* head, const char* usr);

void freeNode (accNode_t* p);

void freeNodeWithoutData (accNode_t* p);

void freeLinkedList (accNode_t* head);

void freeLinkedListWithoutData (accNode_t* head);

int removeAccount (accNode_t* head, const char* usr);

int overrideAhead (accNode_t* curr, const accNode_t* nextNode);

// int removeFromList (accNode_t* head, const char* usr);

// void freeNodeExAcc (accNode_t* p);

chat_msg_node_t* createMsgNode (chat_msg_t* acc);

chat_msg_node_t* createMsgLinkedList ();

chat_msg_node_t* insertMsgNodeHead (chat_msg_node_t* head, chat_msg_node_t* inode);

chat_msg_node_t* reverseMsgLinkedList (chat_msg_node_t* head);

void freeMsgNode (chat_msg_node_t* p);

void freeMsgLinkedList (chat_msg_node_t* head);

#endif // !LINKEDLIST_H
