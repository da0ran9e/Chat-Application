#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <ctype.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "type.h"
#include "config.h"
#include "utility.h"
#include "linkedList.h"
#include "io.h"
#include <time.h>
#include <poll.h>
#include <limits.h>
#include <pthread.h>
#include <syslog.h>
#include <errno.h>

#define BACKLOG 20

char servAddr[30];
int servPort;
int udpTrackingPort;
int listenfd;

accNode_t* accLst;
accNode_t* onlineLst;

pthread_mutex_t onlineLstMutex;
pthread_mutex_t accLstMutex;

int processLogic (int connfd) {
    struct sockaddr_in cliaddr;
    socklen_t addrSize = sizeof(cliaddr);
    int recvBytes, sentBytes;
    time_t nowSec;
    account_t* reqAcc;          // bless god dont never ever free this pointer in this func
    int loginState;
    struct tm* currentTime;
    char timeStr[30];
    net_msg_t requestMsg;
    net_msg_t responseMsg;
    accNode_t* foundNode;
    accNode_t* foundNodeOnlLst;
    accNode_t* p;

    getpeername(connfd, &cliaddr, &addrSize);

    syslog(LOG_DEBUG, "Connected to client [%s:%d] on connection fd = %d", inet_ntoa(cliaddr.sin_addr), htons(cliaddr.sin_port), connfd);
    recvBytes = recvMsg(connfd, &requestMsg);
    if (recvBytes > 0) {
        reqAcc = (account_t*) malloc(sizeof(*reqAcc));
        memset(reqAcc, 0, sizeof(*reqAcc));
        responseMsg.len = 0;

        nowSec = time(NULL);
        currentTime = localtime(&nowSec);
        sprintf(timeStr, "%02d:%02d:%02d %04d/%02d/%02d", currentTime->tm_hour, currentTime->tm_min, currentTime->tm_sec,
                                                    currentTime->tm_year + 1900, currentTime->tm_mon, currentTime->tm_mday);
        switch(requestMsg.type) {
            case LOGIN:
                deserializeAuthAccount(reqAcc, requestMsg.payload);
                syslog(LOG_DEBUG, "Received LOGIN message (usr = %s, strlen(pwd) = %d, usrAddr = (%s:%d))",
                        reqAcc->usr, strlen(reqAcc->pwd), inet_ntoa(reqAcc->usrAddr.sin_addr), htons(reqAcc->usrAddr.sin_port));
                pthread_mutex_lock(&accLstMutex);
                foundNode = searchAccount(accLst, reqAcc->usr);
                if (foundNode != NULL) {    // registered account
                    syslog(LOG_DEBUG, "Found usr %s, status = %hi", foundNode->acc->usr, foundNode->acc->status);
                    if (foundNode->acc->status == ACTIVE) {     // active account
                        if (strcmp(reqAcc->pwd, foundNode->acc->pwd) == 0) {    // new login
                            pthread_mutex_lock(&onlineLstMutex);
                            foundNodeOnlLst = searchAccount(onlineLst, reqAcc->usr);
                            pthread_mutex_unlock(&onlineLstMutex);
                            if (foundNodeOnlLst == NULL) {
                                responseMsg.type = LOGINSUCCESS;
                                
                                foundNode->acc->logCnt = 0;
                                
                                genToken(reqAcc->token);
                                strcpy(foundNode->acc->token, reqAcc->token);
                                reqAcc->pwd[0] = 0;
                                responseMsg.len = serializeAccessAccount(responseMsg.payload, reqAcc);

                                p = createNode(reqAcc);
                                pthread_mutex_lock(&onlineLstMutex);
                                insertNodeHead(onlineLst, p);
                                pthread_mutex_unlock(&onlineLstMutex);

                                syslog(LOG_INFO, "User: %s. Successfully login at %s (Access token: %s)", reqAcc->usr, timeStr, reqAcc->token);
                            } else {    // already logged in
                                responseMsg.type = LOGINALREADY;
                                syslog(LOG_INFO, "User %s tried to login with other machine at %s", reqAcc->usr, timeStr);
                            }
                        } else {
                            foundNode->acc->logCnt++;
                            syslog(LOG_DEBUG, "Wrong identity. Login count = %d (MAXRETRY = %d)", foundNode->acc->logCnt, MAXRETRY);
                            if (foundNode->acc->logCnt > MAXRETRY) {
                                foundNode->acc->status = LOCKED;
                                responseMsg.type = LOCKEDACC;
                                syslog(LOG_INFO, "User: \"%s\". Entered wrong password at %s. %d retry(ies) left. LOCKED account", reqAcc->usr, timeStr, MAXRETRY - foundNode->acc->logCnt);
                            } else {
                                responseMsg.type = WRONGIDENT;
                                syslog(LOG_INFO, "User: \"%s\". Entered wrong password at %s. %d retry(ies) left", reqAcc->usr, timeStr, MAXRETRY - foundNode->acc->logCnt);
                            }
                        }
                    } else if (foundNode->acc->status == INACTIVE) {
                        responseMsg.type = DISABLED;
                        syslog(LOG_INFO, "User: \"%s\". Inactive account. Attempted at %s", reqAcc->usr, timeStr);
                    } else if (foundNode->acc->status == LOCKED) {
                        responseMsg.type = LOCKEDACC;
                        syslog(LOG_INFO, "User: \"%s\". Locked account. Attempted at %s", reqAcc->usr, timeStr);
                    }
                } else {
                    responseMsg.type = UNREGISTERED;
                    syslog(LOG_INFO, "Cannot find user: \"%s\" in database. Operation at %s", reqAcc->usr, timeStr);
                }
                pthread_mutex_unlock(&accLstMutex);

                sendMsg(connfd, &responseMsg);
                break;
            case LOGOUT:
                deserializeAccessAccount(reqAcc, requestMsg.payload);
                syslog(LOG_DEBUG, "Received LOGOUT message (usr = %s, token = %d)", reqAcc->usr, reqAcc->token);
                foundNode = searchAccount(onlineLst, reqAcc->usr);
                if (foundNode != 0) {
                    if (strcmp(reqAcc->token, foundNode->acc->token) == 0) {
                        syslog(LOG_INFO, "Remove account %s from online list", foundNode->acc->usr);
                        pthread_mutex_lock(&onlineLstMutex);
                        foundNode->acc->token[0] = 0;
                        removeAccount(onlineLst, foundNode->acc->usr);
                        pthread_mutex_unlock(&onlineLstMutex);
                        responseMsg.type = END;
                        responseMsg.len = 0;
                        sendMsg(connfd, &responseMsg);
                    }
                } else {
                    syslog(LOG_INFO, "Cannot find user %s in online list", reqAcc->usr);
                }
                break;
            case ONLINELIST:
                deserializeAccessAccount(reqAcc, requestMsg.payload);
                syslog(LOG_DEBUG, "Received ONLINELIST message (usr = %s, token = %s)", reqAcc->usr, reqAcc->token);
                p = onlineLst->next;
                sentBytes = 1;
                while (p != NULL && sentBytes > 0) {
                    responseMsg.type = ONLINELIST;
                    responseMsg.len = serializeOnlineAccount(responseMsg.payload, p->acc);

                    sentBytes = sendMsg(connfd, &responseMsg);
                    
                    p = p->next;
                }
                if (sentBytes > 0) {
                    responseMsg.type = END;
                    responseMsg.len = 0;
                    sendMsg(connfd, &responseMsg);
                }
                break;
            case REGISTER:
                deserializeSignupAccount(reqAcc, requestMsg.payload);
                printf("|%s|\n", reqAcc->usr);
                syslog(LOG_DEBUG, "Received REGISTER message (usr = %s, strlen(pwd) = %d, usrAddr = (%s:%d))",
                        reqAcc->usr, strlen(reqAcc->pwd), inet_ntoa(reqAcc->usrAddr.sin_addr), htons(reqAcc->usrAddr.sin_port));
                foundNode = searchAccount(accLst, reqAcc->usr);
                if (foundNode == NULL) {
                    reqAcc->status = ACTIVE;
                    pthread_mutex_lock(&accLstMutex);
                    insertNodeHead(accLst, createNode(reqAcc));
                    saveAccountList(accLst, accountFilePath);
                    pthread_mutex_unlock(&accLstMutex);
                    responseMsg.type = REGSUCCESS;
                    responseMsg.len = 0;
                    sendMsg(connfd, &responseMsg);
                    syslog(LOG_INFO, "Add account %s to account list", reqAcc->usr);
                } else {
                    responseMsg.type = REGDUP;
                    responseMsg.len = 0;
                    sendMsg(connfd, &responseMsg);
                }
                break;
            case VERIFY:
                deserializeAccessAccount(reqAcc, requestMsg.payload);
                foundNode = searchAccount(onlineLst, reqAcc->usr);
                memset(&responseMsg, 0, sizeof(responseMsg));
                responseMsg.type = INVALID;
                responseMsg.len = 0;
                if (foundNode != NULL && strcmp(foundNode->acc->token, reqAcc->token) == 0) {
                    responseMsg.type = VALID;
                    responseMsg.len = serializeChatAccount(responseMsg.payload, foundNode->acc);
                }
                sendMsg(connfd, &responseMsg);
                break;
            default:
                syslog(LOG_DEBUG, "From client (%s:%d) recevied unhandled message type = %d", inet_ntoa(cliaddr.sin_addr), htons(cliaddr.sin_port), requestMsg.type);
                responseMsg.type = BADREQ;
                responseMsg.len = 0;
                sendMsg(connfd, &responseMsg);
                break;
        }
    }

    return recvBytes;
}

int startServer () {
    int nready, i, maxi, port, listenfd, connfd, sockfd;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    const int OPEN_MAX = sysconf(_SC_OPEN_MAX);  // maximum number of opened files
    struct pollfd clients[OPEN_MAX];
    ssize_t n;
    int INFTIM = -1;
    int on;

    syslog(LOG_DEBUG, "OPEN_MAX = %d", OPEN_MAX);

    // Create listen socket
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        syslog(LOG_ERR, "Error socket : %s", strerror(errno));
        return EXIT_FAILURE;
    } else {
        syslog(LOG_INFO, "Create listen socket %d", listenfd);
        // for debug only
        on = 1;
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    }

    // Initialize server socket address
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(servAddr);
    servaddr.sin_port = htons(servPort);

    // Bind socket to an address
    if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        syslog(LOG_ERR, "Bind failed on address %s:%d", servAddr, servPort);
        perror("Bind TCP error: ");
        return EXIT_FAILURE;
    }
    printf("TCP server start on address (%s:%d)\n", servAddr, servPort);

    // Listen
    if (listen(listenfd, BACKLOG) < 0) {
        syslog(LOG_ERR, "Listen failed on socket %d", listenfd);
        return EXIT_FAILURE;
    }
    syslog(LOG_INFO, "Server started on address %s:%d", servAddr, servPort);

    clients[0].fd = listenfd;
    clients[0].events = POLLRDNORM;

    for (i = 1; i < OPEN_MAX; i++) {
        clients[i].fd = -1;  // -1 indicates available entry
    }
    maxi = 0;  // max index into clients[] array

    while (1) {
        nready = poll(clients, maxi + 1, INFTIM);

        if (nready <= 0) {
            continue;
        }

        // Check new connection
        if (clients[0].revents & POLLRDNORM) {
            clilen = sizeof(cliaddr);
            if ((connfd = accept(listenfd, (struct sockaddr*)&cliaddr,
                                 &clilen)) < 0) {
                syslog(LOG_ERR, "Error: accept %s", strerror(errno));
                return EXIT_FAILURE;
            }

            syslog(LOG_INFO, "Accept socket %d (%s:%hu)", connfd, inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));

            // Save client socket into clients array
            for (i = 0; i < OPEN_MAX; i++) {
                if (clients[i].fd < 0) {
                    clients[i].fd = connfd;
                    break;
                }
            }

            // No enough space in clients array
            if (i == OPEN_MAX) {
                syslog(LOG_WARNING, "Force closing socket due to exceed OPEN_MAX clients (OPEN_MAX = %d)", OPEN_MAX);
                close(connfd);
            }

            clients[i].events = POLLRDNORM;

            if (i > maxi) {
                maxi = i;
            }

            // No more readable file descriptors
            if (--nready <= 0) {
                continue;
            }
        }

        // Check all clients to read data
        for (i = 1; i <= maxi; i++) {
            if ((connfd = clients[i].fd) < 0) {
                continue;
            }

            // If the client is readable or errors occur
            if (clients[i].revents & (POLLRDNORM | POLLERR)) {
                n = processLogic(connfd);
                if (n < 0) {
                    syslog(LOG_WARNING, "Error from socket %d: %s", connfd, strerror(errno));
                    close(connfd);
                    clients[i].fd = -1;
                } else if (n == 0) {  // connection closed by client
                    syslog(LOG_INFO, "Socket %d was closed by client", connfd);
                    close(connfd);
                    clients[i].fd = -1;
                }

                // No more readable file descriptors
                if (--nready <= 0) {
                    break;
                }
            }
        }
    }

    close(listenfd);
    return EXIT_SUCCESS;
}

void* maintainOnlineLst (void* none) {
    pthread_detach(pthread_self());

    accNode_t* p;
    accNode_t* tmp;
    
    while (1) {
        pthread_mutex_lock(&onlineLstMutex);
        p = onlineLst->next;
        while (p != NULL) {
            tmp = p->next;
            p->acc->offCnt++;
            if (p->acc->offCnt > MAXOFFLINE) {
                syslog(LOG_INFO, "Maintain Online List - Removed user %s from online list", p->acc->usr);
                removeAccount(onlineLst, p->acc->usr);
            }
            p = tmp;
        }       
        pthread_mutex_unlock(&onlineLstMutex);

        sleep(MAINTAININTV);
    }
}

void* hiMsgTracking (void* none) {
    pthread_detach(pthread_self());

    int listenfd, recvBytes;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len = sizeof(cliaddr);
    char msg[PAYLOADSIZE];
    account_t sender;
    accNode_t* foundNode;

    if ((listenfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        syslog(LOG_ERR, "HI message tracking - Error socket : %s", strerror(errno));
        return EXIT_FAILURE;
    } else {
        syslog(LOG_INFO, "HI message tracking - Created listen socket %d", listenfd);
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(servAddr);
    servaddr.sin_port = htons(udpTrackingPort);
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) >= 0) {
        syslog(LOG_INFO, "HI message tracking - Started on address %s:%d", servAddr, udpTrackingPort);
        printf("UDP Tracking server start on address (%s:%d)\n", servAddr, udpTrackingPort);
    } else {
        syslog(LOG_ERR, "HI message tracking - Bind failed on address %s:%d (stderr: %s)", servAddr, udpTrackingPort, strerror(errno));
        perror("Bind UDP error: ");
        exit(EXIT_FAILURE);
    }

    while (1) {
        recvBytes = recvfrom(listenfd, msg, PAYLOADSIZE, 0, (struct sockaddr*)&cliaddr, &len);
        if (recvBytes > 0) {
            extractHIMsg(sender.usr, sender.token, msg);
            pthread_mutex_lock(&onlineLstMutex);
            foundNode = searchAccount(onlineLst, sender.usr);
            if (foundNode != NULL) {
                if (strcmp(foundNode->acc->token, sender.token) == 0) {
                    foundNode->acc->offCnt = 0;
                } else {
                    syslog(LOG_DEBUG, "HI message tracking - Token does not match usr: %s", sender.usr);
                }
            } else {
                syslog(LOG_DEBUG, "HI message tracking - User is not in online list: %s", sender.usr);
            }
            pthread_mutex_unlock(&onlineLstMutex);
        }
    }

    close(listenfd);
}

int main(int argc, char** argv) {
    pthread_t tid1, tid2;
    
    openlog("P2P Chat Server", LOG_PID, LOG_USER);

    if (argc < 4) {
        printf("You must specify server address, port number for server TCP and UDP");
        return 0;
    }
    strcpy(servAddr, argv[1]);
    servPort = atoi(argv[2]);
    udpTrackingPort = atoi(argv[3]);
    
    accLst = createNode(NULL);
    onlineLst = createNode(NULL);
    
    readAccountList(accLst, accountFilePath);
    
    pthread_mutex_init(&accLstMutex, NULL);
    pthread_mutex_init(&onlineLstMutex, NULL);


    pthread_create(&tid1, NULL, &maintainOnlineLst, NULL);
    pthread_create(&tid2, NULL, &hiMsgTracking, NULL);
    
    startServer();
    
    freeLinkedList(accLst);
    freeLinkedList(onlineLst);
    pthread_mutex_destroy(&accLstMutex);
    pthread_mutex_destroy(&onlineLstMutex);
    closelog();
    
    return 0;
}
