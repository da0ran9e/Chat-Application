#include <stdio.h>
#include "type.h"
#include "utility.h"
#include "linkedList.h"
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>

accNode_t* readAccountList (accNode_t* head, char* filePath) {
    FILE* fp = fopen(filePath, "r");
    char err[200];
    account_t* acc;
    accNode_t* node;
    int ret;

    if (fp != NULL) {
        while(!feof(fp)) {
            acc = (account_t*) malloc(sizeof(*acc));
            memset(acc, 0, sizeof(*acc));
            ret = fscanf(fp, "%s%s%hhd", acc->usr, acc->pwd, &acc->status);
            if (ret < 3) {
                free(acc);
                break;
            }
            acc->logCnt = 0;
            node = createNode(acc);
            insertNodeHead(head, node);
        }
    } else {
        sprintf(err, "Cannot open %s for reading ", filePath);
        perror(err);
    }

    fclose(fp);
    return head;
}

int saveAccountList (accNode_t* head, char* filePath) {
    FILE* fp = fopen(filePath, "w");
    int ret = 0;
    accNode_t* i;
    char err[200];

    if (fp != NULL) {
        i = head->next;
        while(i != NULL) {
            fprintf(fp, "%s %s %d\n", i->acc->usr, i->acc->pwd, i->acc->status);
            i = i->next;
        }
    } else {
        sprintf(err, "Cannot open %s for writing ", filePath);
        perror(err);
        ret = -1;
    }

    fclose(fp);
    return ret;
}

chat_msg_node_t* readConversation (chat_msg_node_t* head, char* currUsr, char* opponentUsr, char* filePath) {
    FILE* fp = fopen(filePath, "rb");
    char err[200];
    chat_msg_t* msg;
    char buf[PAYLOADSIZE];
    int msgLen;
    int insert = 0;
    int issuccess;

    if (fp != NULL) {
        msg = (chat_msg_t*) malloc(sizeof(*msg));
        while(!feof(fp)) {
            issuccess = fread(&msgLen, sizeof(msgLen), 1, fp);
            if (issuccess) {
                insert = 0;
                fread(buf, 1, msgLen, fp);
                deserializeChatMsgStorage(msg, buf, msgLen);
                if ((strcasecmp(msg->from, currUsr) == 0 && strcasecmp(msg->to, opponentUsr) == 0)
                    ||(strcasecmp(msg->from, opponentUsr) == 0 && strcasecmp(msg->to, currUsr) == 0)) {
                    insertMsgNodeHead(head, createMsgNode(msg));
                    msg = (chat_msg_t*) malloc(sizeof(*msg));
                    insert = 1;
                }
            }
        }
        if (!insert) {
            free(msg);
        }
        fclose(fp);
    } else {
        sprintf(err, "Cannot open %s for reading ", filePath);
        perror(err);
    }

    reverseMsgLinkedList(head);
    return head;
}

int appendChatFile (chat_msg_t* msg, char* filePath) {
    FILE* fp = fopen(filePath, "ab");
    int ret = 0;
    char buf[PAYLOADSIZE];
    int msgLen;
    char err[200];

    if (fp != NULL) {
        msgLen = serializeChatMsgStorage(buf, msg);
        fwrite(&msgLen, sizeof(msgLen), 1, fp);
        fwrite(buf, 1, msgLen, fp);
        fclose(fp);
    } else {
        sprintf(err, "Cannot open %s for writing ", filePath);
        perror(err);
        ret = -1;
    }

    return ret;
}

// return number of bytes sent
int sendBuffer (int sockfd, const char* buffer, int bufferLen) {
    int nLeft, idx;
    int ret;
    int retry = 0;

    nLeft = bufferLen;
    idx = 0;

    while (nLeft > 0) {
        // Assume sockfd is a valid, connected stream socket
        ret = send(sockfd, &buffer[idx], nLeft, 0);
        if (ret == -1) {
            ++retry;
            if (retry > 10) {
                perror("Cannot send data ");
                break;
            }
        } else {
            nLeft -= ret;
            idx += ret;
            retry = 0;
        }
    }

    return ret >= 0 ? idx : ret;
}

int recvBuffer (int sockfd, char* buffer, int msgLen) {
    int ret, nLeft, idx;
    char tmpBuff[BUFFSIZE];
    
    nLeft = msgLen;  // length of the data needs to be received
    idx = 0;
    while (nLeft > 0) {
        ret = recv(sockfd, &tmpBuff, nLeft > BUFFSIZE ? BUFFSIZE : nLeft, 0);
        if (ret == -1) {
            // Error handler
            break;
        }
        memcpy(buffer + idx, tmpBuff, ret);
        idx += ret;
        nLeft -= ret;
    }

    return ret >= 0 ? idx : ret;
}

int sendMsg (int sockfd, const net_msg_t* msg) {
    char msgBuff[sizeof(net_msg_t)];
    int msgLen;
    int sentBytes;

    msgLen = serializeNetMsg(msgBuff, msg);
    sentBytes = send(sockfd, &msgLen, sizeof(msgLen), 0);
    if (sentBytes > 0) {
        sentBytes = sendBuffer(sockfd, msgBuff, msgLen);
    }

    return sentBytes;
}

int recvMsg (int sockfd, net_msg_t* msg) {
    char msgBuff[sizeof(net_msg_t)];
    int msgLen;
    int recvBytes;

    recvBytes = recv(sockfd, &msgLen, sizeof(msgLen), 0);
    if (recvBytes > 0) {
        recvBytes = recvBuffer(sockfd, msgBuff, msgLen);
        if (recvBytes > 0) {
            msg = deserializeNetMsg(msg, msgBuff);
        }
    }

    return recvBytes;
}