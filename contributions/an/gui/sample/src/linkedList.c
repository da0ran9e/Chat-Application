#include "type.h"
#include <stdlib.h>
#include <string.h>
#include "linkedList.h"

accNode_t* createNode (account_t* acc) {
    accNode_t* p = (accNode_t*) malloc(sizeof(accNode_t));
    p->acc = acc;
    p->next = NULL;

    return p;
}

accNode_t* createLinkedList () {
    accNode_t* head = (accNode_t*) malloc(sizeof(accNode_t));
    head->acc = NULL;
    head->next = NULL;

    return head;
}

accNode_t* insertNode (accNode_t* head, accNode_t* inode) {
    accNode_t* i = head;
    while (i->next != NULL) {
        i = i->next;
    }
    i->next = inode;

    return inode;
}

accNode_t* insertNodeHead (accNode_t* head, accNode_t* inode) {
    inode->next = head->next;
    head->next = inode;

    return head;
}

accNode_t* searchAccount (accNode_t* head, const char* usr) {
    accNode_t* i = head->next;
    while (i != NULL) {
        if (i->acc != NULL && !strcasecmp(i->acc->usr, usr)) {
            return i;
        }
        i = i->next;
    }

    return NULL;
}

void freeNode (accNode_t* p) {
    if (p == NULL) return;
    free(p->acc);
    free(p);
}

void freeNodeWithoutData (accNode_t* p) {
    if (p == NULL) return;
    free(p);
}

void freeLinkedList (accNode_t* head) {
    accNode_t* i = head;
    while (i != NULL) {
        accNode_t* p = i->next;
        freeNode(i);
        i = p;
    }
}

void freeLinkedListWithoutData (accNode_t* head) {
    accNode_t* i = head;
    while (i != NULL) {
        accNode_t* p = i->next;
        freeNodeWithoutData(i);
        i = p;
    }
}

int removeAccount (accNode_t* head, const char* usr) {
    accNode_t* i = head;
    accNode_t* p;
    while(i->next != NULL) {
        if (!strcasecmp(i->next->acc->usr, usr)) {
            p = i->next->next;
            freeNode(i->next);
            i->next = p;
            return 0;
        }
        i = i->next;
    }

    return 1;
}

int overrideAhead (accNode_t* curr, const accNode_t* nextNode) {
    if (curr == NULL) {
        return -1;
    }
    if (curr->next == NULL) {
        curr->next = nextNode;
    } else {
        freeNode(curr->next);
        curr->next = nextNode;
    }

    return 0;
}

// int removeFromList (accNode_t* head, const char* usr) {
//     accNode_t* i = head;
//     accNode_t* p;

//     while(i->next != NULL) {
//         if (!strcmp(i->next->acc->usr, usr)) {
//             p = i->next->next;
//             freeNodeExAcc(i->next);
//             i->next = p;
//             return 0;
//         }
//         i = i->next;
//     }

//     return 1;
// }

chat_msg_node_t* createMsgNode (chat_msg_t* msg) {
    chat_msg_node_t* p = (chat_msg_node_t*) malloc(sizeof(chat_msg_node_t));
    p->msg = msg;
    p->next = NULL;

    return p;
}

chat_msg_node_t* createMsgLinkedList () {
    chat_msg_node_t* head = (chat_msg_node_t*) malloc(sizeof(chat_msg_node_t));
    head->msg = NULL;
    head->next = NULL;

    return head;
}

chat_msg_node_t* insertMsgNodeHead (chat_msg_node_t* head, chat_msg_node_t* inode) {
    inode->next = head->next;
    head->next = inode;

    return head;
}

chat_msg_node_t* reverseMsgLinkedList (chat_msg_node_t* head) {
    chat_msg_node_t* left;
    chat_msg_node_t* curr;
    chat_msg_node_t* right;

    curr = head->next;
    // head -> left -> curr -> right
    if (curr != NULL) {
        left = NULL;
        while (curr->next != NULL) {
            right = curr->next;
            curr->next = left;
            
            left = curr;
            curr = right;
        }
        curr->next = left;
        head->next = curr;
    }

    return head;
}

void freeMsgNode (chat_msg_node_t* p) {
    if (p == NULL) return;
    free(p->msg);
    free(p);
}

void freeMsgLinkedList (chat_msg_node_t* head) {
    chat_msg_node_t* i = head;
    chat_msg_node_t* p;

    while (i != NULL) {
        p = i->next;
        freeMsgNode(i);
        i = p;
    }
}