#ifndef  _EXTENTION_H
#define  _EXTENTION_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "publish.h"
#include "subscribe.h"
#include "global.h"
#include "db.h"

// 声明一个发布链表和一个订阅链表
extern pNode pHead;
extern ssNode ssHead;
extern sqlite3 *db;
// 处理从客户端接收的消息
char cmsg[5][22];

int init(char *ip, int port);
void clientMsg(char *str);
void writeRecord(char *name, char *action, char *result);
void handleMsg(char *str, pMsg msg);
void handleClient(char type, int sk, char action, char *title, char *content);
void *while_thread(void *);
#endif
