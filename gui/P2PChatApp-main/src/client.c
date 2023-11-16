#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include "config.h"
#include "utility.h"
#include "type.h"
#include "io.h"
#include <syslog.h>
#include <errno.h>
#include <pthread.h>
#include <poll.h>
#include <time.h>
#include <ctype.h>
#include <gtk/gtk.h>
#include "guiUtility.h"
#include <sys/fcntl.h>>

#define BACKLOG 20

char cliAddr[30];
char servAddr[30];
int servPort;
int servUdpPort;

int serverSockFD = -1;
account_t currAcc;
net_msg_t authMsg;          // prepared msg.len and msg.token for authentication

accNode_t* onlineLst;
accNode_t* chatLst;
char hiMsg[PAYLOADSIZE];    // prepared HI message
int hiMsgLen;

int loginState = NONE;
int cliSockState = NA;
int clientState = NA;
int logoutCount = 0;

pthread_mutex_t onlineLstMutex;
pthread_mutex_t chatLstMutex;
pthread_mutex_t logoutMutex;

GtkWidget* globalStartupWnd;
GtkWidget* globalListWnd;

void quitApp (GList *freeLst) {
    int *sockfd;
    GtkWindow* startupWnd;

    if (freeLst != NULL) {
        sockfd = g_list_nth_data(freeLst, 0);
        startupWnd = g_list_nth_data(freeLst, 3);

        close(*sockfd);
        g_list_free(g_steal_pointer(&freeLst));
    }
    closelog();
    pthread_mutex_destroy(&onlineLstMutex);
    pthread_mutex_destroy(&chatLstMutex);
    freeLinkedList(onlineLst);
    free(chatLst);
}

void printSentMsg (chat_msg_t* msg) {
    struct tm* sendTime;
    char sendTimeStr[30];

    sendTime = localtime(&msg->at);
    trimRight(asctime_r(sendTime, sendTimeStr));
    printf("<-- To %s (%s): %s\n", msg->to, sendTimeStr, msg->txt);
}

void printReceivedMsg (chat_msg_t* msg) {
    struct tm* sendTime;
    char sendTimeStr[30];

    sendTime = localtime(&msg->at);
    trimRight(asctime_r(sendTime, sendTimeStr));
    printf("--> From %s (%s): %s\n", msg->from, sendTimeStr, msg->txt);
}

void sprintSentMsg (char* buf, chat_msg_t* msg) {
    struct tm* sendTime;
    char sendTimeStr[30];

    sendTime = localtime(&msg->at);
    trimRight(asctime_r(sendTime, sendTimeStr));
    sprintf(buf, "To %s (%s): %s\n", msg->to, sendTimeStr, msg->txt);

    return buf;
}

void sprintReceivedMsg (char* buf, chat_msg_t* msg) {
    struct tm* sendTime;
    char sendTimeStr[30];

    sendTime = localtime(&msg->at);
    trimRight(asctime_r(sendTime, sendTimeStr));
    sprintf(buf, "From %s (%s): %s\n", msg->from, sendTimeStr, msg->txt);

    return buf;
}

int showWidget (gpointer data) {
    gtk_widget_show(GTK_WIDGET(data));

    return 0;
}

void insertMsgGUI (GtkVBox* chatVBox, chat_msg_t* msg, int direction) {
    char msgStr[MSGLEN+50];
    GtkWidget* msgLbl;
    int height;
    int width;

    if (direction == SEND) {
        sprintSentMsg(msgStr, msg);
    } else {
        sprintReceivedMsg(msgStr, msg);
    }

    msgLbl = gtk_label_new(msgStr);
    // gtk_widget_get_size_request(GTK_WIDGET(msgLbl), &width, &height);
    // gtk_widget_set_size_request(GTK_WIDGET(msgLbl), width, height - 5);
    gtk_label_set_xalign (GTK_LABEL(msgLbl), 0.0);
    gtk_box_pack_start(GTK_BOX(chatVBox), msgLbl, FALSE, FALSE, 0);
    g_idle_add(G_SOURCE_FUNC(showWidget), msgLbl);
}

int getTCPSockfdByStruct (struct sockaddr_in addr) {
    int sockfd;
    int connectFailed;
    char* logPrefix = "Get TCP socket fd";

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        syslog(LOG_ERR, "%s - Error socket: %s", logPrefix, strerror(errno));
        return EXIT_FAILURE;
    }
    syslog(LOG_INFO, "%s - Created socket", logPrefix);

    connectFailed = connect(sockfd, (struct sockaddr_in*) &addr, sizeof(addr));
    if (connectFailed < 0) {
        syslog(LOG_ERR, "%s - Connect failed to %s:%d. Error: %s", logPrefix, inet_ntoa(addr.sin_addr), htons(addr.sin_port), strerror(errno));
        return EXIT_FAILURE;
    }
    syslog(LOG_INFO, "%s - Connect successfully to %s:%d with socket fd = %d", logPrefix, inet_ntoa(addr.sin_addr), htons(addr.sin_port), sockfd);

    return sockfd;
}

int getTCPSockfd (char* addrStr, unsigned short port ) {
    int sockfd;
    struct sockaddr_in addr;
    
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(addrStr);
    addr.sin_port = htons(port);

    sockfd = getTCPSockfdByStruct(addr);

    return sockfd;
}

void* sendHIMsg (void* none) {
    pthread_detach(pthread_self());

    int sockfd;
    struct sockaddr_in servaddr;
    socklen_t addrLen = sizeof(servaddr);
    char* logPrefix = "HI message sending";

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(servAddr);//inet_addr(UDPTRACKINGADDR);
    servaddr.sin_port = htons(servUdpPort);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        syslog(LOG_ERR, "%s - Error socket : %s", logPrefix, strerror(errno));
        return EXIT_FAILURE;
    } else {
        syslog(LOG_INFO, "%s - Created socket %d", logPrefix, sockfd);
    }
    
    while (loginState == LOGINSUCCESS && clientState != EXIT) {
        sendto(sockfd, hiMsg, hiMsgLen+1, 0, (struct sockaddr*)&servaddr, addrLen);

        sleep(HIMSGINTV);
    }

    close(sockfd);
    syslog(LOG_DEBUG, "%s - Closed send HI message socket", logPrefix);
}

int destroyWidget (gpointer data) {
    gtk_widget_destroy(GTK_WIDGET(data));

    return 0;
}

accNode_t** mergeAndFilterOnlLst (accNode_t** changeLst, accNode_t *oldLst, accNode_t *newLst) {
    accNode_t* addLst;
    accNode_t* removeLst;
    accNode_t* p;
    accNode_t* prev;
    accNode_t* tmp;
    accNode_t* foundNode;
    account_t* clone;

    addLst = createLinkedList();
    removeLst = createLinkedList();

    // remove offline user from oldLst
    p = oldLst->next;
    prev = oldLst;
    while (p != NULL) {
        foundNode = searchAccount(newLst, p->acc->usr);
        tmp = p->next;
        if (foundNode != NULL) {
            prev = p;
        } else {
            insertNodeHead(removeLst, createNode(p->acc));
            printf("remove |%s|\n", p->acc->usr);
            prev->next = p->next;
        }

        p = tmp;
    }

    // add new online user to oldLst
    p = newLst->next;
    while (p != NULL) {
        foundNode = searchAccount(oldLst, p->acc->usr);
        if (foundNode == NULL) {
            clone = (account_t*)malloc(sizeof(*clone));
            memcpy(clone, p->acc, sizeof(*clone));
            insertNodeHead(oldLst, createNode(clone));
            insertNodeHead(addLst, createNode(p->acc));
            printf("add |%s|\n", p->acc->usr);
        }

        p = p->next;
    }

    changeLst[0] = addLst;
    changeLst[1] = removeLst;

    return changeLst;
}

void updateBtn (GtkWidget* widget, gpointer data) {
    GtkButton* btn;
    accNode_t* removeLst;
    char* usr;
    accNode_t* foundNode;
    GList* btnChildren;

    btn = GTK_BUTTON(widget);
    removeLst = (accNode_t*) data;
    btnChildren = gtk_container_get_children(GTK_CONTAINER(btn));

    usr = gtk_label_get_text(GTK_LABEL(g_list_nth_data(btnChildren, 0)));
    
    foundNode = searchAccount(removeLst, usr);

    if (foundNode != NULL) {
        // gtk_widget_destroy(btn);
        g_idle_add(G_SOURCE_FUNC(destroyWidget), (gpointer)btn);
    }
}

void onClickSendBtn (GtkWidget* emitter, gpointer data) {
    GList* sendMsgGList;
    int sockfd;
    account_t* connAcc;
    GtkVBox* chatVBox;
    GtkEntry* chatEty;
    char* logPrefix = "Send chat message GUI";
    chat_msg_t* chatMsg;
    net_msg_t requestMsg;
    int sentBytes;
    char path[500];
    char printMsg[MSGLEN+50];
    char prompt[100];
    accNode_t* foundNode;

    sendMsgGList = (GList*)data;
    connAcc = (account_t*)g_list_nth_data(sendMsgGList, 0);
    chatEty = GTK_ENTRY(g_list_nth_data(sendMsgGList, 1));
    chatMsg = (chat_msg_t*)g_list_nth_data(sendMsgGList, 2);

    foundNode = searchAccount(chatLst, connAcc->usr);
    if (foundNode == NULL) {
        sprintf(prompt, "User '%s' has left the chat", connAcc->usr);
        errDialog(gtk_widget_get_toplevel(emitter), prompt);
        return;
    }

    chatVBox = connAcc->chatVBox;
    syslog(LOG_DEBUG, "%s - Send to address (%s:%hu)", logPrefix, inet_ntoa(connAcc->usrAddr.sin_addr), ntohs(connAcc->usrAddr.sin_port));
    
    strcpy(chatMsg->txt, gtk_entry_get_text(chatEty));
    chatMsg->at = time(NULL);

    requestMsg.type = CHATMSG;
    requestMsg.len = serializeChatMsg(requestMsg.payload, chatMsg);

    sockfd = getTCPSockfdByStruct(connAcc->usrAddr);
    if (sockfd > 0) {
        sentBytes = sendMsg(sockfd, &requestMsg);
        syslog(LOG_DEBUG, "%s - User %s sent to %s along with token = %s (sentBytes = %d)", logPrefix, currAcc.usr, connAcc->usr, currAcc.token, sentBytes);
        if (sentBytes > 0) {
            // TODO add sent message in chat window
            insertMsgGUI(chatVBox, chatMsg, SEND);
            appendChatFile(chatMsg, getMsgFilePath(path, currAcc.usr));
            gtk_entry_set_text(chatEty, "");
        } else if (sentBytes == 0) {
            sprintf(prompt, "User '%s' has left the chat", connAcc->usr);
            errDialog(gtk_widget_get_toplevel(emitter), prompt);   
        } else {
            errDialog(gtk_widget_get_toplevel(emitter), "Connection error!");
        }
        close(sockfd);
    } else {
            errDialog(gtk_widget_get_toplevel(emitter), "Connection error!");
    }
}

void onCloseChatWnd (GtkWidget* emitter, GdkEvent* event, gpointer data) {
    GList* chatGLst;
    GtkWidget* chatWnd;
    account_t* connAcc;
    chat_msg_t* chatMsg;
    net_msg_t requestMsg;
    int sockfd;

    if (data != NULL) {
        chatGLst = (GList*)data;
        connAcc = (account_t*)g_list_nth_data(chatGLst, 0);
        chatMsg = (chat_msg_t*)g_list_nth_data(chatGLst, 2);
        chatWnd = GTK_WIDGET(g_list_nth_data(chatGLst, 3));

        memcpy(&requestMsg, &authMsg, sizeof(requestMsg));
        sockfd = getTCPSockfdByStruct(connAcc->usrAddr);
        if (sockfd > 0) {
            requestMsg.type = DISCONNECT;
            sendMsg(sockfd, &requestMsg);
            close(sockfd);
        }
        pthread_mutex_lock(&chatLstMutex);
        removeAccount(chatLst, connAcc->usr);
        pthread_mutex_unlock(&chatLstMutex);
        free(chatMsg);
        free(connAcc);
        // g_list_free(chatGLst);
        // g_idle_add(G_SOURCE_FUNC(gtk_widget_destroy), (gpointer)chatWnd);
        gtk_widget_destroy(chatWnd);
    }
}

int showChatWnd (gpointer data) {
    gtk_widget_show_all(GTK_WIDGET(data));

    return 0;
}

void initMessage (GtkVBox* chatVBox, account_t* connAcc) {
    chat_msg_node_t* head;
    chat_msg_node_t* p;
    char path[500];

    head = createMsgLinkedList();
    head = readConversation(head, currAcc.usr, connAcc->usr, getMsgFilePath(path, currAcc.usr));
    p = head->next;
    while (p != NULL) {
        if (strcasecmp(p->msg->from, currAcc.usr) == 0) {
            insertMsgGUI(chatVBox, p->msg, SEND);
        } else {
            insertMsgGUI(chatVBox, p->msg, RECV);
        }

        p = p->next;
    }
    freeMsgLinkedList(head);
}

void sendChatMsgGUI(GList* sendChatGLst) {
    GtkBuilder* builder;
    GtkWindow* chatWnd;
    GtkScrolledWindow* chatScrolledWnd;
    GtkVBox* chatVBox;
    GtkLabel* chatLbl;
    GtkButton* sendBtn;
    GtkEntry* chatEty;
    GList* sendMsgGLst;
    account_t* connAcc;
    account_t* cloneAcc;
    chat_msg_t* chatMsg;
    char prompt[100];

    connAcc = (account_t*)g_list_nth_data(sendChatGLst, 0);

    builder = gtk_builder_new_from_file("glade/chat.glade");

	chatWnd = GTK_WINDOW(gtk_builder_get_object(builder, "chatWnd"));
    chatScrolledWnd = GTK_SCROLLED_WINDOW(gtk_builder_get_object(builder, "chatScrolledWnd"));
    sendBtn = GTK_WIDGET(gtk_builder_get_object(builder,"sendBtn"));
	chatEty = GTK_WIDGET(gtk_builder_get_object(builder,"chatEntry"));
    chatLbl = GTK_LABEL(gtk_builder_get_object(builder, "chatLbl"));
    chatVBox = GTK_VBOX(gtk_vbox_new(FALSE, 0));

	gtk_container_add(GTK_CONTAINER(chatScrolledWnd), GTK_WIDGET(chatVBox));
    connAcc->chatVBox = chatVBox;
    connAcc->chatWnd = chatWnd;
    cloneAcc = (account_t*)malloc(sizeof(*cloneAcc));
    memcpy(cloneAcc, connAcc, sizeof(*cloneAcc));
    sprintf(prompt, "Chat with %s", cloneAcc->usr);
    gtk_label_set_text(chatLbl, prompt);

    chatMsg = (chat_msg_t*)malloc(sizeof(*chatMsg));
    strcpy(chatMsg->from, currAcc.usr);
    strcpy(chatMsg->token, currAcc.token);
    strcpy(chatMsg->to, cloneAcc->usr);
    
    sendMsgGLst = NULL;
    sendMsgGLst = g_list_append(sendMsgGLst, cloneAcc);
    sendMsgGLst = g_list_append(sendMsgGLst, chatEty);
    sendMsgGLst = g_list_append(sendMsgGLst, chatMsg);
    sendMsgGLst = g_list_append(sendMsgGLst, chatWnd);

    initMessage(chatVBox, cloneAcc);
    g_signal_connect(sendBtn, "clicked", G_CALLBACK(onClickSendBtn), sendMsgGLst);
    g_signal_connect(chatWnd, "delete-event", G_CALLBACK(onCloseChatWnd), sendMsgGLst);
    g_idle_add(G_SOURCE_FUNC(showChatWnd), (gpointer)chatWnd);


    g_object_unref(builder);
}

void waitingDlgHandler (GtkWidget* emitter, int responseId, gpointer data) {
    int* waitingDlgResponse;

    waitingDlgResponse = (int*)data;
    *waitingDlgResponse = responseId;
}

void* connectChatHandler (void* data) {
    account_t* connAcc;
    accNode_t* p;
    int sockfd;
    int* sockfdp;
    char* logPrefix = "Connect chat handler";
    net_msg_t requestMsg;
    net_msg_t responseMsg;
    int recvBytes, sentBytes;
    int* waitingDlgResponse;
    int hasErr;
    GList* sendChatGLst;
    char prompt[200];
    GList* connectChatClickGLst;
    GtkDialog* waitingDlg;

    connectChatClickGLst = (GList*)data;
    connAcc = (account_t*)g_list_nth_data(connectChatClickGLst, 0);
    waitingDlgResponse = (int*)g_list_nth_data(connectChatClickGLst, 1);
    waitingDlg = GTK_DIALOG(g_list_nth_data(connectChatClickGLst, 2));

    sockfd = getTCPSockfdByStruct(connAcc->usrAddr);
    if (sockfd > 0) {
        syslog(LOG_INFO, "%s - Connect to %s by socket fd = %d", logPrefix, connAcc->usr, sockfd);
        memcpy(&requestMsg, &authMsg, sizeof(authMsg));
        requestMsg.type = CONNECT;
        // printf("[*] Wait for user %s respond...\n", acc->usr);
        
        sentBytes = sendMsg(sockfd, &requestMsg);
        if (sentBytes > 0) {
            hasErr = 0;
            recvBytes = recvMsg(sockfd, &responseMsg);
            if (recvBytes > 0) {
                if (responseMsg.type == ACK) {
                    responseMsg.type = VERIFY;
                    sentBytes = sendMsg(serverSockFD, &responseMsg);
                    if (sentBytes > 0) {
                        recvBytes = recvMsg(serverSockFD, &responseMsg);
                        if (recvBytes > 0) {
                            switch (responseMsg.type) {
                                case INVALID:
                                    hasErr = 1;
                                    break;
                                case VALID:
                                    deserializeChatAccount(connAcc, responseMsg.payload);
                                    break;
                                default:
                                    break;
                            }
                        } else {
                            hasErr = 1;
                        }
                    } else {
                        hasErr = 1;
                    }
                } else {
                    hasErr = 1;
                }
            } else {
                hasErr = 1;
            }
            *waitingDlgResponse = GTK_RESPONSE_NONE;
            if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK) < 0) {
                syslog(LOG_ERR, "%s - fcntl on socket %d error", logPrefix, sockfd);
                hasErr = 1;
            }
            do {
                recvBytes = recvMsg(sockfd, &responseMsg);
                if (recvBytes == 0) {
                    hasErr = 1;
                } else if (recvBytes < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        usleep(50000);      // sleep 50ms to reduce resoure consuming
                    } else {
                        syslog(LOG_ERR, "%s - Socket error", logPrefix);
                        hasErr = 1;
                    }
                }
            } while (*waitingDlgResponse != GTK_RESPONSE_CANCEL && !hasErr && recvBytes <= 0);
            g_idle_add(G_SOURCE_FUNC(destroyWidget), (gpointer)waitingDlg);
            // gtk_widget_destroy(GTK_WIDGET(waitingDlg));
            if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) & ~O_NONBLOCK) < 0) {
                syslog(LOG_ERR, "%s - fcntl on socket %d error", logPrefix, sockfd);
                hasErr = 1;
            }
            if (recvBytes > 0 && !hasErr && *waitingDlgResponse != GTK_RESPONSE_CANCEL) {
                switch (responseMsg.type) {
                    case ACCEPT:
                        connAcc->chatVBox = NULL;
                        connAcc->chatWnd = NULL;
                        p = createNode(connAcc);
                        pthread_mutex_lock(&chatLstMutex);
                        insertNodeHead(chatLst, p);
                        pthread_mutex_unlock(&chatLstMutex);
                        // TODO notify main thread to draw chat window
                        // clientState = CHATTING;
                        // printf("[+] User %s accepted to chat. Moving to chat interface...\n", connAcc->usr);
                        // close(sockfd);
                        sendChatGLst = NULL;
                        sendChatGLst = g_list_append(sendChatGLst, connAcc);
                        // sendChatMsg((void*)connAcc);
                        sendChatMsgGUI(sendChatGLst);
                        g_list_free(sendChatGLst);
                        break;
                    case REJECT:
                        // TODO notify main thread for rejection
                        syslog(LOG_INFO, "%s - User %s send REJECT message", logPrefix, connAcc->usr);
                        sprintf(prompt, "User '%s' refused to chat", connAcc->usr);
                        errDialog(globalListWnd, prompt);
                        // printf("[-] User %s rejected to chat.\n", connAcc->usr);
                        break;
                    default:
                        syslog(LOG_WARNING, "%s - User %s send undefined response message. Consider rejection", logPrefix, connAcc->usr);
                        // TODO notify main thread for rejection
                        break;

                }
            } else if (recvBytes == 0) {
                syslog(LOG_WARNING, "%s - Cannot receive response message from user %s. Consider rejection", logPrefix, connAcc->usr);
                sprintf(prompt, "User '%s' is no longer online", connAcc->usr);
                // TODO notify main thread for unable to connect
                errDialog(globalListWnd, prompt);
            } else if (*waitingDlgResponse == GTK_RESPONSE_CANCEL) {
                // sprintf(prompt, "User cancel request");
                // errDialog(globalListWnd, prompt);
            } else {
                errDialog(globalListWnd, "Connection error!");
            }
        } else if (sentBytes == 0) {
            syslog(LOG_WARNING, "%s - Cannot send connect message to user %s", logPrefix, connAcc->usr);
            // TODO notify main thread for unable to connect
            sprintf(prompt, "User '%s' is no longer online", connAcc->usr);
            // TODO notify main thread for unable to connect
            g_idle_add(G_SOURCE_FUNC(destroyWidget), (gpointer)waitingDlg);
            errDialog(globalListWnd, prompt);
        } else {
            g_idle_add(G_SOURCE_FUNC(destroyWidget), (gpointer)waitingDlg);
            errDialog(globalListWnd, "Connection error!");
        }
        close(sockfd);
    } else {
        syslog(LOG_WARNING, "%s - Cannot connect to user %s", logPrefix, connAcc->usr);
        // TODO notify main thread for unable to connect
        g_idle_add(G_SOURCE_FUNC(destroyWidget), (gpointer)waitingDlg);
        errDialog(globalListWnd, "Connection error!");
    }
    
    free(waitingDlgResponse);
    g_list_free(connectChatClickGLst);
}

void connectChatGUI (GtkWidget* emitter) {
    accNode_t* foundNode;
    account_t* acc;
    account_t* connAcc;
    GtkButton* btn;
    char* usr;
    GList* btnChildren;
    char* logPrefix = "Connect chat click";
    pthread_t tid;
    int* waitingDlgResponse;
    GList* sendChatGLst;
    GtkDialogFlags flags;
    char dialogMsg[200];
    GtkWidget* waitingDlg;
    GtkWidget* waitingLbl;
    GtkWidget* waitingContainer;
    GList* connectChatClickGLst;

    btn = GTK_BUTTON(emitter);
    btnChildren = gtk_container_get_children(GTK_CONTAINER(btn));

    usr = gtk_label_get_text(GTK_LABEL(g_list_nth_data(btnChildren, 0)));

    pthread_mutex_lock(&onlineLstMutex);
    foundNode = searchAccount(onlineLst, usr);

    if (foundNode == NULL) {
        errDialog(globalListWnd, "Selected user is offline now");
        return;
    }

    acc = (account_t*)foundNode->acc;
    connAcc = (account_t*)malloc(sizeof(*connAcc));

    memcpy(connAcc, acc, sizeof(*acc));
    pthread_mutex_unlock(&onlineLstMutex);

    flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
    sprintf(dialogMsg, "Wating for user %s's response...", connAcc->usr);
    waitingDlg = gtk_message_dialog_new(globalListWnd, flags, GTK_MESSAGE_INFO, GTK_BUTTONS_CANCEL, NULL);
    gtk_widget_set_size_request(waitingDlg, 450, 100);
    waitingLbl = gtk_label_new(dialogMsg);
    gtk_label_set_xalign(GTK_LABEL(waitingLbl),0.2);
    waitingContainer = gtk_message_dialog_get_message_area(GTK_DIALOG(waitingDlg));
    gtk_container_add(GTK_CONTAINER(waitingContainer), waitingLbl);

    waitingDlgResponse = (int*)malloc(sizeof(int));
    connectChatClickGLst = NULL;
    connectChatClickGLst = g_list_append(connectChatClickGLst, connAcc);
    connectChatClickGLst = g_list_append(connectChatClickGLst, waitingDlgResponse);
    connectChatClickGLst = g_list_append(connectChatClickGLst, waitingDlg);
   
    g_signal_connect(waitingDlg, "response", G_CALLBACK(waitingDlgHandler), waitingDlgResponse);
    gtk_widget_show_all(GTK_WIDGET(waitingDlg));
    pthread_create(&tid, NULL, connectChatHandler, (void*)connectChatClickGLst);
}

void addAccToOnlVBox (GtkVBox *onlVBox, accNode_t* addLst) {
    GtkWidget* usrLbl;
    GtkWidget* usrBtn;
    accNode_t* p;

    p = addLst->next;
    while (p != NULL) {
        usrLbl = gtk_label_new(p->acc->usr);
        gtk_label_set_xalign (GTK_LABEL(usrLbl), 0.0);
        usrBtn = gtk_button_new();
        gtk_container_add(GTK_CONTAINER(usrBtn), usrLbl);
        gtk_box_pack_start(GTK_BOX(onlVBox), usrBtn, FALSE, FALSE, 0);
		g_signal_connect(usrBtn, "clicked", G_CALLBACK(connectChatGUI), NULL);
        g_idle_add(G_SOURCE_FUNC(showWidget), (gpointer)usrBtn);
        g_idle_add(G_SOURCE_FUNC(showWidget), (gpointer)usrLbl);

        p = p->next;
    }
}

void updateOnlLstGUI (GtkVBox *onlVBox, accNode_t** changeLst) {
    accNode_t* addLst;
    accNode_t* removeLst;

    addLst = changeLst[0];
    removeLst = changeLst[1];
    gtk_container_foreach(GTK_CONTAINER(onlVBox), (GtkCallback) updateBtn, removeLst);
    addAccToOnlVBox(onlVBox, addLst);
}

void* updateOnlineLst (void* data) {
    pthread_detach(pthread_self());
    GList* onlGLst = (GList*) data;

    int sockfd;
    account_t* acc;
    net_msg_t requestMsg;
    net_msg_t responseMsg;
    int sentBytes, recvBytes;
    accNode_t* p;
    accNode_t* newLst;
    accNode_t* changeLst[2];
    char* logPrefix = "Update online list";
    GtkVBox* onlVBox;
    int localLogoutCount = logoutCount;     // solving the case when user login in 10s after logging out

    onlVBox = g_list_nth_data(onlGLst, 0);
    sockfd = getTCPSockfd(servAddr, servPort);
    if (sockfd < 0) {
        syslog(LOG_ERR, "%s - Cannot make connection to server", logPrefix);
        // TODO use flag to represent state of server

        return;
    }
    memcpy(&requestMsg, &authMsg, sizeof(authMsg));
    requestMsg.type = ONLINELIST;

    while (loginState == LOGINSUCCESS && clientState != EXIT && localLogoutCount == logoutCount) {
        sentBytes = sendMsg(sockfd, &requestMsg);
        if (sentBytes > 0) {
            newLst = createLinkedList();
            do {
                recvBytes = recvMsg(sockfd, &responseMsg);
                if (recvBytes > 0) {
                    if (responseMsg.type == ONLINELIST) {
                        acc = (account_t*)malloc(sizeof(*acc));
                        deserializeOnlineAccount(acc, responseMsg.payload);
                        if (strcasecmp(acc->usr, currAcc.usr) != 0) {
                            insertNodeHead(newLst, createNode(acc));
                        }
                    } else if (responseMsg.type != END) {
                        syslog(LOG_WARNING, "%s - Server responded message type %hhi", logPrefix, responseMsg.type);
                    }
                }
            } while (recvBytes > 0 && responseMsg.type != END);
            if (responseMsg.type == END) {
                pthread_mutex_lock(&onlineLstMutex);
                mergeAndFilterOnlLst(changeLst, onlineLst, newLst);
                pthread_mutex_unlock(&onlineLstMutex);
                updateOnlLstGUI(onlVBox, changeLst);
                freeLinkedListWithoutData(changeLst[0]);
                freeLinkedList(changeLst[1]);
            }
            freeLinkedList(newLst);
            syslog(LOG_DEBUG, "%s - Updated", logPrefix);
        }

        sleep(ONLINELSTINTV);
    }

    close(sockfd);
    syslog(LOG_DEBUG, "%s - Closed update online list socket fd", logPrefix);
}

void* sendChatMsg (void* accVoid) {
    account_t* acc;
    int sendSockFD;
    net_msg_t requestMsg;
    chat_msg_t chatMsg;
    int stopChatting;
    int sentBytes, recvBytes;
    char msgFormatStr[20];
    struct tm* sendTime;
    char sendTimeStr[30];
    char* logPrefix = "Send chat message";
    chat_msg_node_t* head;
    chat_msg_node_t* p;
    char path[500];
    int dialogResponse;

    acc = (account_t*) accVoid;
    clearCL();
    
    head = createMsgLinkedList();
    head = readConversation(head, currAcc.usr, acc->usr, getMsgFilePath(path, currAcc.usr));
    p = head->next;
    while (p != NULL) {
        if (strcasecmp(p->msg->from, currAcc.usr) == 0) {
            printSentMsg(p->msg);
        } else {
            printReceivedMsg(p->msg);
        }

        p = p->next;
    }
    freeMsgLinkedList(head);

    strcpy(chatMsg.from, currAcc.usr);
    strcpy(chatMsg.token, currAcc.token);
    strcpy(chatMsg.to, acc->usr);
    requestMsg.type = CHATMSG;
    sendSockFD = getTCPSockfdByStruct(acc->usrAddr);
    snprintf(msgFormatStr, 20, "%%%d[^\n]", MSGLEN-1);      // read at most MSGLEN - 1 character
    
    // clearCL();
    syslog(LOG_DEBUG, "%s - Send to address (%s:%hu)", logPrefix, inet_ntoa(acc->usrAddr.sin_addr), ntohs(acc->usrAddr.sin_port));
    stopChatting = 0;
    do {
        printf("[*] Enter message ('exit.' to exit, 'rf.' to refresh): ");
        scanf(msgFormatStr, chatMsg.txt);
        clearBuffer();
        if (strcmp(chatMsg.txt, "exit.") == 0) {   
            clientState = NA;
            syslog(LOG_DEBUG, "%s - Exit chat and return online list", logPrefix);

            pthread_mutex_lock(&chatLstMutex);
            removeAccount(chatLst, acc->usr);
            pthread_mutex_unlock(&chatLstMutex);

            stopChatting = 1;
        } else if (strcmp(chatMsg.txt, "rf.") == 0) {
            
        } else {
            chatMsg.at = time(NULL);
            requestMsg.len = serializeChatMsg(&requestMsg.payload, &chatMsg);
            
            sentBytes = sendMsg(sendSockFD, &requestMsg);
            syslog(LOG_DEBUG, "%s - User %s sent to %s along with token = %s (sentBytes = %d)", logPrefix, currAcc.usr, acc->usr, currAcc.token, sentBytes);
            if (sentBytes > 0) {
                // TODO add sent message in chat window
                printSentMsg(&chatMsg);
                appendChatFile(&chatMsg, getMsgFilePath(path, currAcc.usr));
            } else {
                printf("[x] Cannot send chat message to %s\n", acc->usr);   
            }
        }
    } while (!stopChatting);


}

void reqDlgHandler (GtkWidget* emitter, int response, gpointer responseCaller) {
    *(int*)responseCaller = response;
}

void runReqDlg (gpointer data) {
    GList* reqDlgGLst;
    GtkDialog* reqDlg;
    int* responseId;

    reqDlgGLst = (GList*)data;
    reqDlg = GTK_DIALOG(g_list_nth_data(reqDlgGLst, 0));
    responseId = (int*)(g_list_nth_data(reqDlgGLst, 1));
    gtk_widget_show_all(GTK_WIDGET(reqDlg));
    *responseId = gtk_dialog_run(reqDlg);
    usleep(10000);
    gtk_widget_destroy(reqDlg);

    return 0;
}

void* connectMsgHandler(void* data) {
    int sockfd;
    account_t* sender;
    char reqMsg[200];
    GtkWidget* reqDlg;
    GtkWidget* reqLbl;
    GtkWidget* reqContainer;
    GtkDialogFlags flags;
    GList* connectorGLst;
    GList* sendChatGLst;
    int dlgResponse;
    int sentBytes;
    char* logPrefix = "Connect Message Handler";
    net_msg_t requestMsg;
    char prompt[200];
    GList* reqDlgGLst;

    connectorGLst = (GList*)data;
    sockfd = *(int*)g_list_nth_data(connectorGLst, 0);
    sender = (account_t*)g_list_nth_data(connectorGLst, 1);

    flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
    reqDlg = gtk_message_dialog_new(globalListWnd, flags, GTK_MESSAGE_INFO, GTK_BUTTONS_YES_NO, NULL);
    gtk_widget_set_size_request(reqDlg, 450, 100);
    sprintf(reqMsg, "User '%s' want to chat with you", sender->usr);
    reqLbl = gtk_label_new(reqMsg);
    gtk_label_set_xalign(GTK_LABEL(reqLbl), 0.2);
    reqContainer = gtk_message_dialog_get_message_area(GTK_DIALOG(reqDlg));
    gtk_container_add(GTK_CONTAINER(reqContainer), reqLbl);

    // gtk_widget_show_all(GTK_WIDGET(reqDlg));
    dlgResponse = GTK_RESPONSE_NONE;
    reqDlgGLst = NULL;
    reqDlgGLst = g_list_append(reqDlgGLst, reqDlg);
    reqDlgGLst = g_list_append(reqDlgGLst, &dlgResponse);
    g_idle_add(G_SOURCE_FUNC(runReqDlg), (gpointer)reqDlgGLst);
    
    while (dlgResponse == GTK_RESPONSE_NONE) {
        usleep(10000);      // pause thread to wait for selection
    }
    g_list_free(reqDlgGLst);
    switch (dlgResponse) {
        case GTK_RESPONSE_YES:
            memcpy(&requestMsg, &authMsg, sizeof(authMsg));
            requestMsg.type = ACCEPT;
            sentBytes = sendMsg(sockfd, &requestMsg);
            if (sentBytes > 0) {
                syslog(LOG_DEBUG, "%s - Accepted connect from %s (%s:%hu)", logPrefix, sender->usr, inet_ntoa(sender->usrAddr.sin_addr), ntohs(sender->usrAddr.sin_port));
                printf("Accept\n");
                sender->chatVBox = NULL;
                sender->chatWnd = NULL;
                pthread_mutex_lock(&chatLstMutex);
                insertNodeHead(chatLst, createNode(sender));
                pthread_mutex_unlock(&chatLstMutex);
                
                sendChatGLst = NULL;
                sendChatGLst = g_list_append(sendChatGLst, sender);
                sendChatMsgGUI(sendChatGLst);
                g_list_free(sendChatGLst);
            } else if (sentBytes == 0) {
                sprintf(prompt, "User '%s' is no longer online", sender->usr);
                errDialog(globalListWnd, prompt);
                free(sender);
            } else {
                sprintf(prompt, "User '%s' cancelled the request", sender->usr);
                errDialog(globalListWnd, prompt);
                free(sender);
            }
            break;
        case GTK_RESPONSE_NO:                
            printf("Reject\n");
            memcpy(&requestMsg, &authMsg, sizeof(authMsg));
            requestMsg.type = REJECT;
            sentBytes = sendMsg(sockfd, &requestMsg);
            if (sentBytes > 0) {
                syslog(LOG_DEBUG, "%s - Rejected connect from %s (%s:%hu)", logPrefix, sender->usr, inet_ntoa(sender->usrAddr.sin_addr), ntohs(sender->usrAddr.sin_port));
            }
            free(sender);
            break;
        default:
            printf("Default\n");
            break;
    }

    g_list_free(connectorGLst);
}

void exitChatWnd (gpointer chatWnd) {
    // g_signal_emit_by_name(chatWnd, "delete-event");
}

int clientMsgHandler (int sockfd) {
    net_msg_t requestMsg;
    net_msg_t responseMsg;
    chat_msg_t chatMsg;
    accNode_t* nodeSender;
    account_t* sender;
    int recvBytes, sentBytes;
    char* logPrefix = "Client message handler";
    char path[500];
    GList* connectorGLst;
    int* sockfdp;
    pthread_t tid;
    int hasErr;
    char prompt[200];

    recvBytes = 0;
    if (loginState == LOGINSUCCESS) {
        recvBytes = recvMsg(sockfd, &requestMsg);
        if (recvBytes > 0) {     
            switch (requestMsg.type) {
                case CHATMSG:
                    deserializeChatMsg(&chatMsg, requestMsg.payload, requestMsg.len);
                    
                    syslog(LOG_DEBUG, "%s - Receive CHATMSG from %s with token %s", logPrefix, chatMsg.from, chatMsg.token);
                    
                    nodeSender = searchAccount(chatLst, chatMsg.from);
                    if (nodeSender != NULL) {
                        sender = nodeSender->acc;
                        if (strcmp(sender->token, chatMsg.token) == 0) {
                            // TODO Do some actions to show message on the GUI
                            // printReceivedMsg(&chatMsg);
                            insertMsgGUI(sender->chatVBox, &chatMsg, RECV);
                            appendChatFile(&chatMsg, getMsgFilePath(path, currAcc.usr));
                        } else {
                            syslog(LOG_WARNING, "%s - Received CHATMSG from invalid sender", logPrefix);
                        }
                    } else {
                        syslog(LOG_WARNING, "%s - Received CHATMSG from unconnected sender", logPrefix);
                    }

                    break;
                case CONNECT:
                    hasErr = 0;
                    // TODO prompt dialog to get iser 
                    sender = (account_t*)malloc(sizeof(*sender));
                    deserializeAccessAccount(sender, requestMsg.payload);
                    // TODO check sender's token with server
                    if (serverSockFD > 0) {
                        requestMsg.type = VERIFY;
                        sentBytes = sendMsg(serverSockFD, &requestMsg);
                        if (sentBytes > 0) {
                            recvBytes = recvMsg(serverSockFD, &responseMsg);
                            if (recvBytes > 0) {
                                switch (responseMsg.type) {
                                    case INVALID:
                                        hasErr = 1;
                                        break;
                                    case VALID:
                                        deserializeChatAccount(sender, responseMsg.payload);
                                        break;
                                    default:
                                        break;
                                }
                            } else {
                                hasErr = 1;
                            }
                        } else {
                            hasErr = 1;
                        }
                    } else {
                        hasErr = 1;
                    }
                    
                    // pthread_mutex_lock(&onlineLstMutex);
                    // nodeSender = searchAccount(onlineLst, sender->usr);
                    // if (nodeSender != NULL && !hasErr) {
                    //     memcpy(&sender->usrAddr, &nodeSender->acc->usrAddr, sizeof(sender->usrAddr));
                    // } else {
                    //     hasErr = 1;
                    // }
                    // pthread_mutex_unlock(&onlineLstMutex);

                    if (!hasErr) {
                        memcpy(&responseMsg, &authMsg, sizeof(responseMsg));
                        responseMsg.type = ACK;
                        sendMsg(sockfd, &responseMsg);
                    }

                    if (!hasErr) {
                        sockfdp = (int*)malloc(sizeof(int));
                        *sockfdp = sockfd;
                        connectorGLst = NULL;
                        connectorGLst = g_list_append(connectorGLst, sockfdp);
                        connectorGLst = g_list_append(connectorGLst, sender);
                        pthread_create(&tid, NULL, connectMsgHandler, (void*)connectorGLst);
                    }
                    // sleep(3);
                    // if (nodeSender != NULL) {
                    //     memcpy(&requestMsg, &authMsg, sizeof(authMsg));
                    //     requestMsg.type = ACCEPT;
                    //     sentBytes = sendMsg(sockfd, &requestMsg);
                    //     if (sentBytes > 0) {
                    //         syslog(LOG_DEBUG, "%s - Accept connect from %s (%s:%hu)", logPrefix, sender->usr, inet_ntoa(sender->usrAddr.sin_addr), ntohs(sender->usrAddr.sin_port));
                            
                    //         pthread_mutex_lock(&chatLstMutex);
                    //         insertNodeHead(chatLst, createNode(sender));
                    //         pthread_mutex_unlock(&chatLstMutex);
                    //         clientState = CHATTING;
                    //     }
                    // }
                    break;
                case DISCONNECT:
                    sender = (account_t*)malloc(sizeof(*sender));
                    deserializeAccessAccount(sender, requestMsg.payload);
                    nodeSender = searchAccount(chatLst, sender->usr);
                    if (nodeSender != NULL) {
                        if (strcmp(nodeSender->acc->token, sender->token) == 0) {
                            sprintf(prompt, "User '%s' has left the chat", sender->usr);
                            errDialog(sender->chatWnd, prompt);
                            pthread_mutex_lock(&chatLstMutex);
                            removeAccount(chatLst, sender->usr);
                            pthread_mutex_unlock(&chatLstMutex);
                        } else {
                            syslog(LOG_WARNING, "%s - Received DISCONNECT from invalid sender", logPrefix);
                        }
                    } else {
                        syslog(LOG_WARNING, "%s - Received DISCONNECT from unconnected sender", logPrefix);
                    }
                    close(sockfd);
                    free(sender);
                    break;
                default:
                    syslog(LOG_WARNING, "%s - Received unhandled message", logPrefix);
                    break;
            }
        }
    } else {
        recvBytes = -1;
        syslog(LOG_WARNING, "%s - Have not logged in, cannot receive message in communication socket", logPrefix);
    }

    return recvBytes;
}

void* clientMsgReceiver (void* none) {
    int nready, i, maxi, port, connfd, listenfd;
    socklen_t clilen;
    struct sockaddr_in cliaddr;
    const int OPEN_MAX = sysconf(_SC_OPEN_MAX);  // maximum number of opened files
    struct pollfd clients[OPEN_MAX];
    ssize_t n;
    int INFTIM = -1;
    int on;
    int bindState;
    char* logPrefix = "Client message receiver";

    cliSockState = NA;

    // Create listen socket
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        syslog(LOG_ERR, "%s - Error socket : %s", strerror(errno));
        clientState = EXIT;
        return;
    } else {
        syslog(LOG_INFO, "%s - Create listen socket fd = %d", logPrefix, listenfd);
        // for debug only
        on = 1;
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    }

    // Initialize server socket address
    memset(&currAcc.usrAddr, 0, sizeof(currAcc.usrAddr));


    currAcc.usrAddr.sin_family = AF_INET;
    currAcc.usrAddr.sin_addr.s_addr = inet_addr(cliAddr); //htonl(INADDR_ANY); //inet_addr(CLIADDR);

    do {
        currAcc.usrAddr.sin_port = htons((unsigned short)rangeRand(20000, 30000));

        // Bind socket to an address
        bindState = bind(listenfd, (struct sockaddr*)&currAcc.usrAddr, sizeof(currAcc.usrAddr));
        if (bindState < 0) {
            syslog(LOG_ERR, "%s - Bind failed on address %s:%d", logPrefix, inet_ntoa(currAcc.usrAddr.sin_addr), htons(currAcc.usrAddr.sin_port));
        }
    } while (bindState < 0);
    printf("Client start on address (%s:%d)\n", inet_ntoa(currAcc.usrAddr.sin_addr), htons(currAcc.usrAddr.sin_port));
    // Listen
    if (listen(listenfd, BACKLOG) < 0) {
        syslog(LOG_ERR, "%s - Listen failed on socket %d", logPrefix, listenfd);
        return;
    }
    cliSockState = READY;
    syslog(LOG_INFO, "%s - Listening on address %s:%d", logPrefix, inet_ntoa(currAcc.usrAddr.sin_addr), htons(currAcc.usrAddr.sin_port));

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
                syslog(LOG_ERR, "%s - Error: accept %s", logPrefix, strerror(errno));
                clientState = EXIT;
                return;
            }

            syslog(LOG_INFO, "%s - Accept socket %d (%s:%hu)", logPrefix, connfd, inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
            // Save client socket into clients array
            for (i = 0; i < OPEN_MAX; i++) {
                if (clients[i].fd < 0) {
                    clients[i].fd = connfd;
                    break;
                }
            }

            // No enough space in clients array
            if (i == OPEN_MAX) {
                syslog(LOG_WARNING, "%s - Force closing socket due to exceed OPEN_MAX clients (OPEN_MAX = %d)", logPrefix, OPEN_MAX);
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
                n = clientMsgHandler(connfd);
                if (n < 0) {
                    syslog(LOG_WARNING, "%s - Error from socket %d: %s", logPrefix, connfd, strerror(errno));
                    close(connfd);
                    clients[i].fd = -1;
                } else if (n == 0) {  // connection closed by client
                    syslog(LOG_INFO, "%s - Socket %d was closed by client", logPrefix, connfd);
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
}

void* connectChat (void* accVoid) {
    pthread_detach(pthread_self());

    account_t* acc;
    account_t* connAcc;
    accNode_t* p;
    int sockfd;
    char* logPrefix = "Connect chat";
    net_msg_t requestMsg;
    net_msg_t responseMsg;
    int recvBytes, sentBytes;

    acc = (account_t*)accVoid;
    connAcc = (account_t*)malloc(sizeof(*connAcc));

    memcpy(connAcc, acc, sizeof(*acc));
    
    sockfd = getTCPSockfdByStruct(connAcc->usrAddr);
    if (sockfd > 0) {
        syslog(LOG_INFO, "%s - Connect to %s by socket fd = %d", logPrefix, connAcc->usr, sockfd);
        clearCL();
        
        memcpy(&requestMsg, &authMsg, sizeof(authMsg));
        requestMsg.type = CONNECT; 
        printf("[*] Wait for user %s respond...\n", acc->usr);
        sentBytes = sendMsg(sockfd, &requestMsg);
        if (sentBytes > 0) {
            recvBytes = recvMsg(sockfd, &responseMsg);
            if (recvBytes > 0) {
                switch (responseMsg.type) {
                    case ACCEPT:
                        // connAcc->sendSockFD = sockfd;
                        p = createNode(connAcc);
                        pthread_mutex_lock(&chatLstMutex);
                        insertNodeHead(chatLst, p);
                        pthread_mutex_unlock(&chatLstMutex);
                        // TODO notify main thread to draw chat window
                        clientState = CHATTING;
                        printf("[+] User %s accepted to chat. Moving to chat interface...\n", connAcc->usr);
                        close(sockfd);
                        sendChatMsg((void*)connAcc);
                        break;
                    case REJECT:
                        // TODO notify main thread for rejection
                        printf("[-] User %s rejected to chat.\n", connAcc->usr);
                        break;
                    default:
                        syslog(LOG_WARNING, "%s - User %s send undefined response message. Consider rejection", logPrefix, connAcc->usr);
                        // TODO notify main thread for rejection
                        break;

                }
            } else {
                syslog(LOG_WARNING, "%s - Cannot receive response message from user %s. Consider rejection", logPrefix, connAcc->usr);
                // TODO notify main thread for unable to connect
            }
        } else {
            syslog(LOG_WARNING, "%s - Cannot send connect message to user %s", logPrefix, connAcc->usr);
            // TODO notify main thread for unable to connect
        }
    } else {
        syslog(LOG_WARNING, "%s - Cannot connect to user %s", logPrefix, connAcc->usr);
        // TODO notify main thread for unable to connect
    }
}

void showOnlineList () {
    accNode_t* p;
    int accCnt;
    account_t accArr[100];
    int choice;
    int endChoice;
    char choiceStr[10];
    pthread_t tid;
    char* logPrefix = "Show online list";

    // TODO make GUI for this
    do {
        accCnt = 0;
        clearCL();
        printf("q. Logout\n");
        printf("0. Refresh\n");
        printf("------------------------\n");

        p = onlineLst;
        pthread_mutex_lock(&onlineLstMutex);
        while (p->next != NULL) {
            if (strcasecmp(p->next->acc->usr, currAcc.usr) != 0) {
                ++accCnt;
                printf("%d. %s\n", accCnt, p->next->acc->usr);
                memcpy(&accArr[accCnt-1], p->next->acc, sizeof(*p->next->acc));
            }
            p = p->next;
        }
        pthread_mutex_unlock(&onlineLstMutex);

        printf("------------------------\n");
        do {
            endChoice = 1;
            printf("[+] Your choice: ");
            scanf("%s", choiceStr);
            clearBuffer();
            if(isalpha(choiceStr[0])) {
                choice = choiceStr[0];
            } else if (isdigit(choiceStr[0])) {
                choice = atoi(choiceStr);
            } else {
                choice = -1;
            }
            if (choice == 'q') {
                // TODO logout
                loginState = LOGOUT;
                // logut ()
            } else if (choice == 0) {
                if (clientState == CHATTING) {
                    pthread_create(&tid, NULL, &sendChatMsg, (void*)chatLst->next->acc);          
                    while (clientState == CHATTING) {
                        sleep(1);
                    }
                }
            } else if (1 <= choice && choice <= accCnt) {
                // TODO connect with user choice
                syslog(LOG_DEBUG, "%s - Send connect request to %s", logPrefix, accArr[choice-1].usr);
                pthread_create(&tid, NULL, &connectChat, (void*)&accArr[choice-1]);
                clientState = CHATTING;
                while (clientState == CHATTING) {
                    sleep(1);
                }
            } else {
                printf("[x] Invalid option, enter again!\n");
                endChoice = 0;
            }
        } while (endChoice == 0);
    } while (loginState == LOGINSUCCESS && clientState != EXIT);
}

void minimizeApp (GtkWidget* emitter, GList* onlGLst) {
    // GtkWindow* listWindow = GTK_WINDOW(g_list_nth_data(onlGLst, 1));
    // GdkWindow* gdkWnd = gtk_widget_get_window(GTK_WIDGET(listWindow));
    // gdk_window_get_state()
}

void logout (GtkWidget* emitter, GList* loginGLst) {
    int sockfd;
    GtkWindow* loginWnd;
    GtkWindow* listWnd;
    GList* onlGLst;
    net_msg_t logoutMsg;
    int sentBytes;

    loginState = LOGOUT;
    ++logoutCount;
    sockfd = *(int*)g_list_nth_data(loginGLst, 0);
    loginWnd = GTK_WINDOW(g_list_nth_data(loginGLst, 1));
    listWnd = GTK_WINDOW(g_list_nth_data(loginGLst, 2));
    onlGLst = (GList*)g_list_nth_data(loginGLst, 3);

    // TODO send logout message
    memcpy(&logoutMsg, &authMsg, sizeof(logoutMsg));
    logoutMsg.type = LOGOUT;
    sentBytes = sendMsg(sockfd, &logoutMsg);
    if (sentBytes > 0) {
        recvMsg(sockfd, &logoutMsg);
    }

    g_list_free(g_steal_pointer(&onlGLst));
    g_list_free(g_steal_pointer(&loginGLst));
    pthread_mutex_lock(&onlineLstMutex);
    freeLinkedList(onlineLst->next);
    onlineLst->next = NULL;
    pthread_mutex_unlock(&onlineLstMutex);
    pthread_mutex_lock(&chatLstMutex);
    freeLinkedList(chatLst->next);
    chatLst->next = NULL;
    pthread_mutex_unlock(&chatLstMutex);
    gtk_widget_destroy(listWnd);
    gtk_widget_show_all(loginWnd);
}

GList* showOnlineListGUI (GList* loginGLst) {
    GtkBuilder* builder;
    GtkWindow* listWindow;
    GtkWindow* loginWnd;
    GtkScrolledWindow* onlScrolledWnd;
    GtkVBox* onlVBox;
    GtkButton* logoutBtn;
    GtkLabel* helloLbl;
    char helloStr[200];
    GList* onlGLst;
    
    onlGLst = NULL;
    loginWnd = GTK_WINDOW(g_list_nth_data(loginGLst, 1));
    
    builder = gtk_builder_new_from_file("glade/online-list.glade");

    listWindow = GTK_WINDOW(gtk_builder_get_object(builder, "listWindow"));
    globalListWnd = listWindow;
    logoutBtn = GTK_BUTTON(gtk_builder_get_object(builder, "logoutBtn"));
    onlScrolledWnd = GTK_SCROLLED_WINDOW(gtk_builder_get_object(builder, "onlScrolledWnd"));
    helloLbl = GTK_LABEL(gtk_builder_get_object(builder, "helloLbl"));
    onlVBox = GTK_VBOX(gtk_vbox_new(FALSE, 1));

    strcpy(helloStr, "Hello, ");
    strcat(helloStr, currAcc.usr);
    strcat(helloStr, "!");
    gtk_label_set_text(helloLbl, helloStr);
	gtk_container_add(GTK_CONTAINER(onlScrolledWnd), GTK_WIDGET(onlVBox));

    onlGLst = g_list_append(onlGLst, onlVBox);
    onlGLst = g_list_append(onlGLst, listWindow);

    loginGLst = g_list_append(loginGLst, listWindow);
    loginGLst = g_list_append(loginGLst, onlGLst);

    // g_signal_connect(listWindow,"destroy",G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(listWindow,"delete_event",G_CALLBACK(minimizeApp), onlGLst);
    g_signal_connect(logoutBtn,"clicked",G_CALLBACK(logout), loginGLst);


    gtk_widget_hide(GTK_WIDGET(loginWnd));
    gtk_widget_show_all(GTK_WIDGET(listWindow));

    g_object_unref(builder);

    return onlGLst;
}

void loginSuccess (GList* loginGLst) {
    pthread_t tid1, tid2;
    GList* onlGLst;
    int* sockfd;

    // prepare for sending HI message and
    onlGLst = NULL;
    loginState = LOGINSUCCESS;
    hiMsgLen = makeHIMsg(hiMsg, currAcc.usr, currAcc.token);
    // send HI message
    pthread_create(&tid1, NULL, &sendHIMsg, NULL);
    // update online list each ONLINELISTINTV secs

    // TODO do something after successfully logging in

    onlGLst = showOnlineListGUI(loginGLst);

    pthread_create(&tid2, NULL, &updateOnlineLst, (void*)onlGLst);
    // while (loginState == LOGINSUCCESS && clientState != EXIT) {
    //     showOnlineList();
    // }

}

void login (int sockfd) {
    int sentBytes;
    char* pwd;
    net_msg_t requestMsg;
    net_msg_t responseMsg;
    char* logPrefix = "Login";
    account_t tmp;
    
    clearCL();
    printf("Enter username: ");
    if (fgets(currAcc.usr, USRLEN, stdin) == NULL) {}
    trim(currAcc.usr);
    pwd = getpass("Enter password: ");
    strcpy(currAcc.pwd, pwd);
    trim(currAcc.pwd);
    syslog(LOG_DEBUG, "%s - Identity: usr = %s, strlen(pwd) = %d), address (%s:%d)", logPrefix, currAcc.usr, strlen(currAcc.pwd),
            inet_ntoa(currAcc.usrAddr.sin_addr), htons(currAcc.usrAddr.sin_port));


    requestMsg.type = LOGIN;
    requestMsg.len = serializeAuthAccount(requestMsg.payload, &currAcc);
    sentBytes = sendMsg(sockfd, &requestMsg);
    
    if (sentBytes > 0) { 
        recvMsg(sockfd, &responseMsg);
        switch (responseMsg.type) {
            case LOGINSUCCESS:
                deserializeAccessAccount(&tmp, responseMsg.payload);
                syslog(LOG_DEBUG, "%s - Received LOGINSUCCESS message with token (usr = %s, token = %s)", logPrefix, tmp.usr, tmp.token);
                authMsg.len = responseMsg.len;
                memcpy(authMsg.payload, responseMsg.payload, responseMsg.len);
                memcpy(currAcc.token, tmp.token, TOKENLEN+1);
                // loginSuccess(sockfd);
                break;
            case LOGINALREADY:
                syslog(LOG_DEBUG, "%s - Received LOGINALREADY message", logPrefix);
                // TODO show you have logged in with other machine
                break;
            case UNREGISTERED:
                syslog(LOG_DEBUG, "%s - Received UNREGISTERED message", logPrefix);
                // TODO show your account you entered does not exist 
                break;
            case WRONGIDENT:
                syslog(LOG_DEBUG, "%s - Received WRONGINDENT message", logPrefix);
                // TODO show you have entered wrong password
                break;
            case LOCKEDACC:
                syslog(LOG_DEBUG, "%s - Received LOCKEDACC message", logPrefix);
                // TODO show your account has been locked because of too many failed login attempts. Contact admin to resolve
                break;
            case DISABLED:
                syslog(LOG_DEBUG, "%s - Received DISABLED message", logPrefix);
                // TODO show your account was no longer available
                break;
            default:
                syslog(LOG_DEBUG, "%s - Received unexpected message type with type code = %d", logPrefix, responseMsg.type);
                // TODO show unexpected error occured, try to login again
                break;
        }
    }
    printf("Press <Enter> to return main menu...");
    getchar();
}

void signup (int sockfd) {
    int sentBytes;
    char* pwd;
    net_msg_t requestMsg;
    net_msg_t responseMsg;
    char* logPrefix = "Sign up";
    account_t tmp;
    char confirmPwd[PWDLEN];
    
    clearCL();
    printf("Enter username: ");
    if (fgets(currAcc.usr, USRLEN, stdin) == NULL) {}
    trim(currAcc.usr);
    pwd = getpass("Enter password: ");
    strcpy(currAcc.pwd, pwd);
    trim(currAcc.pwd);
    pwd = getpass("Confirm password: ");
    strcpy(confirmPwd, pwd);
    trim(confirmPwd);
    syslog(LOG_DEBUG, "%s - Identity: usr = %s, strlen(pwd) = %d, strlen(confirmPwd) = %d, address (%s:%d)", logPrefix, currAcc.usr, strlen(currAcc.pwd), strlen(confirmPwd),
            inet_ntoa(currAcc.usrAddr.sin_addr), htons(currAcc.usrAddr.sin_port));

    if (strcmp(currAcc.pwd, confirmPwd) == 0) {
        requestMsg.type = REGISTER;
        requestMsg.len = serializeSignupAccount(requestMsg.payload, &currAcc);
        sentBytes = sendMsg(sockfd, &requestMsg);
        
        if (sentBytes > 0) { 
            recvMsg(sockfd, &responseMsg);
            switch (responseMsg.type) {
                case REGSUCCESS:
                    syslog(LOG_DEBUG, "%s - Received REGSUCCESS message", logPrefix);
                    printf("Successfully signed up username '%s' !\n", currAcc.usr);
                    break;
                case REGDUP:
                    syslog(LOG_DEBUG, "%s - Received REGDUP message", logPrefix);
                    printf("Username '%s' was not available !\n", currAcc.usr);
                    break;
                default:
                    syslog(LOG_DEBUG, "%s - Received unexpected message type with type code = %d", logPrefix, responseMsg.type);
                    // TODO show unexpected error occured, try to login again
                    break;
            }
        } else {
            syslog(LOG_DEBUG, "%s - Cannot send message to server", logPrefix);
            printf("Internal server error!\n", currAcc.usr);
        }
    } else {
        syslog(LOG_DEBUG, "%s - Confirmed password is not matched (usr = '%s')", logPrefix, currAcc.usr);
        printf("Confirmed password is not matched!\n", currAcc.usr);
    }

    printf("Press <Enter> to return main menu...");
    getchar();
}

void menu () {
    int endChoice;
    int choice;
    char choiceStr[0];
    int sockfd;

    sockfd = getTCPSockfd(servAddr, servPort);
    serverSockFD = sockfd;
    do {
        clearCL();
        printf("1. Sign up\n");
        printf("2. Login\n");
        printf("0. Quit\n");
        printf("------------------------\n");
        do {
            endChoice = 1;
            printf("[+] Your choice: ");
            scanf("%s", choiceStr);
            clearBuffer();
            if(isalpha(choiceStr[0])) {
                choice = choiceStr[0];
            } else if (isdigit(choiceStr[0])) {
                choice = atoi(choiceStr);
            } else {
                choice = -1;
            }

            if (choice == 0) {
                clientState = EXIT;
            } else if (choice == 1) {
                signup(sockfd);
            } else if (choice == 2) {
                login(sockfd);
            } else {
                printf("[x] Invalid option, enter again!\n");
                endChoice = 0;
            }
        } while (endChoice == 0);
    } while (clientState != EXIT);
    close(sockfd);
}

void loginGUI (GtkWidget* emitter, GList* infoGLst) {
    int sockfd;
    int sentBytes;
    net_msg_t requestMsg;
    net_msg_t responseMsg;
    char* logPrefix = "Login";
    account_t tmp;
    GtkEntry* usrEty;
    GtkEntry* pwdEty;
    GList* loginGLst;

    loginGLst = NULL;

    sockfd = *((int*)g_list_nth_data(infoGLst, 0));
    usrEty = GTK_ENTRY(g_list_nth_data(infoGLst, 1));
    pwdEty = GTK_ENTRY(g_list_nth_data(infoGLst, 2));
    strcpy(currAcc.usr, gtk_entry_get_text(usrEty));
    strcpy(currAcc.pwd, gtk_entry_get_text(pwdEty));
    trim(currAcc.usr);
    trim(currAcc.pwd);

    syslog(LOG_DEBUG, "%s - Identity: usr = %s, strlen(pwd) = %d), address (%s:%d)", logPrefix, currAcc.usr, strlen(currAcc.pwd),
            inet_ntoa(currAcc.usrAddr.sin_addr), htons(currAcc.usrAddr.sin_port));

    requestMsg.type = LOGIN;
    requestMsg.len = serializeAuthAccount(requestMsg.payload, &currAcc);
    sentBytes = sendMsg(sockfd, &requestMsg);
    
    if (sentBytes > 0) { 
        recvMsg(sockfd, &responseMsg);
        switch (responseMsg.type) {
            case LOGINSUCCESS:
                deserializeAccessAccount(&tmp, responseMsg.payload);
                syslog(LOG_DEBUG, "%s - Received LOGINSUCCESS message with token (usr = %s, token = %s)", logPrefix, tmp.usr, tmp.token);
                authMsg.len = responseMsg.len;
                memcpy(authMsg.payload, responseMsg.payload, responseMsg.len);
                memcpy(currAcc.token, tmp.token, TOKENLEN+1);

                loginGLst = g_list_append(loginGLst, g_list_nth_data(infoGLst, 0));     // sockfd
                loginGLst = g_list_append(loginGLst, g_list_nth_data(infoGLst, 3));     // startupWnd
                loginSuccess(loginGLst);
                break;
            case LOGINALREADY:
                syslog(LOG_DEBUG, "%s - Received LOGINALREADY message", logPrefix);
                // TODO show you have logged in with other machine
                errDialog(emitter, "You have logged in on other machine");
                break;
            case UNREGISTERED:
                syslog(LOG_DEBUG, "%s - Received UNREGISTERED message", logPrefix);
                // TODO show your account you entered does not exist 
                errDialog(emitter, "You account have not been unregistered");
                break;
            case WRONGIDENT:
                syslog(LOG_DEBUG, "%s - Received WRONGINDENT message", logPrefix);
                // TODO show you have entered wrong password
                errDialog(emitter, "You have entered wrong username or password");
                break;
            case LOCKEDACC:
                syslog(LOG_DEBUG, "%s - Received LOCKEDACC message", logPrefix);
                // TODO show your account has been locked because of too many failed login attempts. Contact admin to resolve
                break;
            case DISABLED:
                syslog(LOG_DEBUG, "%s - Received DISABLED message", logPrefix);
                // TODO show your account was no longer available
                break;
            default:
                syslog(LOG_DEBUG, "%s - Received unexpected message type with type code = %d", logPrefix, responseMsg.type);
                // TODO show unexpected error occured, try to login again
                break;
        }
    } else {
        printf("Cannot connect to the server!\n");
        errDialog(emitter, "Cannot connect to the server");
    }
}

void onCloseSignupWnd (GtkWidget* emitter, GdkEvent* event, gpointer data) {
    GList* signupGLst;
    GtkWidget* signupWnd;
    GtkWidget* startupWnd;

    signupGLst = (GList*)data;

    startupWnd = GTK_WIDGET(g_list_nth_data(signupGLst, 4));
    signupWnd = GTK_WIDGET(g_list_nth_data(signupGLst, 5));

    gtk_widget_show(startupWnd);
    gtk_widget_destroy(signupWnd);
}

void onClickSignupBtn (GtkWidget* emitter, gpointer data) {
    GList* signupGLst;
    GtkWidget* signupWnd;
    GtkWidget* startupWnd;
    GtkWidget* usrEty;
    GtkWidget* pwdEty;
    GtkWidget* cpwdEty;
    int sentBytes;
    net_msg_t requestMsg;
    net_msg_t responseMsg;
    char* logPrefix = "Sign up GUI";
    char confirmPwd[PWDLEN];
    char prompt[100];
    int sockfd;

    signupGLst = (GList*)data;
    sockfd = *(int*)g_list_nth_data(signupGLst, 0);
    usrEty = GTK_WIDGET(g_list_nth_data(signupGLst, 1));
    pwdEty = GTK_WIDGET(g_list_nth_data(signupGLst, 2));
    cpwdEty = GTK_WIDGET(g_list_nth_data(signupGLst, 3));
    startupWnd = GTK_WIDGET(g_list_nth_data(signupGLst, 4));
    signupWnd = GTK_WIDGET(g_list_nth_data(signupGLst, 5));

    strcpy(currAcc.usr, gtk_entry_get_text(GTK_ENTRY(usrEty)));
    trim(currAcc.usr);
    if (strlen(currAcc.usr) == 0) {
        errDialog(emitter, "You must enter username");
        return;
    }
    strcpy(currAcc.pwd, gtk_entry_get_text(GTK_ENTRY(pwdEty)));
    trim(currAcc.pwd);
    if (strlen(currAcc.pwd) == 0) {
        errDialog(emitter, "You must enter password");
        return;
    }
    strcpy(confirmPwd, gtk_entry_get_text(GTK_ENTRY(cpwdEty)));
    trim(confirmPwd);
    if (strcmp(currAcc.pwd, confirmPwd) != 0) {
        errDialog(emitter, "Confirm password does not match");
        return;
    }
    
    syslog(LOG_DEBUG, "%s - Identity: usr = %s, strlen(pwd) = %d, strlen(confirmPwd) = %d, address (%s:%d)", logPrefix, currAcc.usr, strlen(currAcc.pwd), strlen(confirmPwd),
            inet_ntoa(currAcc.usrAddr.sin_addr), htons(currAcc.usrAddr.sin_port));

    requestMsg.type = REGISTER;
    requestMsg.len = serializeSignupAccount(requestMsg.payload, &currAcc);
    sentBytes = sendMsg(sockfd, &requestMsg);
    
    if (sentBytes > 0) { 
        recvMsg(sockfd, &responseMsg);
        switch (responseMsg.type) {
            case REGSUCCESS:
                syslog(LOG_DEBUG, "%s - Received REGSUCCESS message", logPrefix);
                sprintf(prompt, "Successfully signed up username '%s' !", currAcc.usr);
                messageDialog(emitter, prompt, GTK_MESSAGE_INFO, 0);
                gtk_widget_show(startupWnd);
                gtk_widget_destroy(signupWnd);
                break;
            case REGDUP:
                syslog(LOG_DEBUG, "%s - Received REGDUP message", logPrefix);
                sprintf(prompt, "Username '%s' was not available !\n", currAcc.usr);
                errDialog(emitter, prompt);
                break;
            default:
                syslog(LOG_DEBUG, "%s - Received unexpected message type with type code = %d", logPrefix, responseMsg.type);
                // TODO show unexpected error occured, try to login again
                errDialog(emitter, "Server error");
                break;
        }
    } else {
        syslog(LOG_DEBUG, "%s - Cannot send message to server", logPrefix);
        // printf("Internal server error!\n", currAcc.usr);
        errDialog(emitter, "Server error");
    }
}

void signupGUI (GtkWidget* emitter, gpointer data) {
    GtkBuilder* builder;
    GList* startupGLst;
    GtkWidget* signupWnd;
    GtkWidget* startupWnd;
    GtkWidget* usrEty;
    GtkWidget* pwdEty;
    GtkWidget* cpwdEty;
    GtkWidget* signupBtn;
    GList* signupGLst;
    int* sockfdp;

    startupGLst = (GList*)data;
    sockfdp = (int*)g_list_nth_data(startupGLst, 0);
    startupWnd = GTK_WIDGET(g_list_nth_data(startupGLst, 3));

    builder = gtk_builder_new_from_file("glade/signup.glade");

    signupWnd = GTK_WIDGET(gtk_builder_get_object(builder, "signupWnd"));
    usrEty = GTK_WIDGET(gtk_builder_get_object(builder, "usrEty"));
    pwdEty = GTK_WIDGET(gtk_builder_get_object(builder, "pwdEty"));
    cpwdEty = GTK_WIDGET(gtk_builder_get_object(builder, "cpwdEty"));
    signupBtn = GTK_WIDGET(gtk_builder_get_object(builder, "signupBtn"));

    signupGLst = NULL;
    signupGLst = g_list_append(signupGLst, sockfdp);
    signupGLst = g_list_append(signupGLst, usrEty);
    signupGLst = g_list_append(signupGLst, pwdEty);
    signupGLst = g_list_append(signupGLst, cpwdEty);
    signupGLst = g_list_append(signupGLst, startupWnd);
    signupGLst = g_list_append(signupGLst, signupWnd);

    g_signal_connect(signupBtn, "clicked", G_CALLBACK(onClickSignupBtn), signupGLst);
    g_signal_connect(signupWnd, "delete-event", G_CALLBACK(onCloseSignupWnd), signupGLst);

    gtk_widget_show_all(signupWnd);
    gtk_widget_hide(startupWnd);

    g_object_unref(builder);
}

void startupGUI (int argc, char** argv) {
    GtkBuilder* builder;
    GtkWidget* startupWnd;
    GtkWidget* loginBtn;
    GtkWidget* usnEntry;
    GtkWidget* pwdEntry;
    GtkWidget* signupBtn;
    GList* menuGLst;

    int* sockfd;

    gtk_init(&argc, &argv);
    menuGLst = NULL;
    sockfd = (int*)malloc(sizeof(int));

    *sockfd = getTCPSockfd(servAddr, servPort);
    serverSockFD = *sockfd;

    builder = gtk_builder_new_from_file("glade/startup.glade");
    globalStartupWnd = startupWnd;

    startupWnd = GTK_WIDGET(gtk_builder_get_object(builder, "startupWnd"));
    // Objects in startupWnd
    loginBtn = GTK_WIDGET(gtk_builder_get_object(builder, "loginBtn"));
    signupBtn = GTK_WIDGET(gtk_builder_get_object(builder, "signupBtn"));
    usnEntry = GTK_WIDGET(gtk_builder_get_object(builder, "usnEntry"));
    pwdEntry = GTK_WIDGET(gtk_builder_get_object(builder, "pwdEntry"));

    menuGLst = g_list_append(menuGLst, sockfd);
    menuGLst = g_list_append(menuGLst, usnEntry);
    menuGLst = g_list_append(menuGLst, pwdEntry);
    menuGLst = g_list_append(menuGLst, startupWnd);

    g_signal_connect(loginBtn, "clicked", G_CALLBACK(loginGUI), menuGLst);
    g_signal_connect(signupBtn, "clicked", G_CALLBACK(signupGUI), menuGLst);
    g_signal_connect(startupWnd, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    g_object_unref(builder);

    gtk_widget_show_all(startupWnd);
    
    gtk_main();
    
    close(sockfd);
}

int main(int argc, char **argv) {
    pthread_t tid;
    int cliSockStateCnt = 0;
    int timeCount = 200;

    if (argc < 5) {
        printf("You must specify client address, server address, server TCP port, server UDP port to run\n");
    }

    strcpy(cliAddr, argv[1]);
    strcpy(servAddr, argv[2]);
    servPort = atoi(argv[3]);
    servUdpPort = atoi(argv[4]);

    openlog("P2P Chat Client", LOG_PID, LOG_USER);
    
    onlineLst = createLinkedList(NULL);
    chatLst = createLinkedList(NULL);
    
    pthread_mutex_init(&onlineLstMutex, NULL);
    pthread_mutex_init(&chatLstMutex, NULL);
    pthread_mutex_init(&logoutMutex, NULL);
    
    // thread for receive chat message
    pthread_create(&tid, NULL, &clientMsgReceiver, NULL);

    // wait for at most 200*50ms = 10s to setup client address (store in currAcc.usrAddr)
    while (cliSockState != READY && cliSockStateCnt < timeCount) {
        usleep(50*1000);
        ++cliSockStateCnt;
    }

    if (cliSockStateCnt >= timeCount) {
        printf("[x] Client cannot setup a socket for communication\n");
        syslog(LOG_ERR, "Cannot bind a chat socket");

        return EXIT_FAILURE;
    }

    // menu();      // CLI
    startupGUI(argc, argv);
    
    closelog();
    pthread_mutex_destroy(&onlineLstMutex);
    pthread_mutex_destroy(&chatLstMutex);
    pthread_mutex_destroy(&logoutMutex);
    freeLinkedList(onlineLst);
    free(chatLst);

    return EXIT_SUCCESS;
}
