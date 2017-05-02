#ifndef _GLOBAL_H
#define _GLOBAL_H
//订阅同一话题的设备列表节点
typedef struct subDevList {
        int sockid;                   //socket id
        struct subDevList *next;
}*sdNode;

//订阅列表节点
typedef struct subscribe {
        char name[10];             //话题
        sdNode sdl;             //设备列表
        struct subscribe *next;
}*ssNode;

// 定义所接收消息的结构体
typedef struct msg {
        char type;
        char action;
        char name[10];
        char content[5];
}*pMsg;

// 话题发布列表
typedef struct publish {
        char name[10];
        char content[5];
        struct publish *next;
}*pNode;

// 用户结构体
typedef struct user {
        int id;
        char name[16];
        char password[22];
        char idcard[20];
        char token[20];
}*pUser;

#include <sqlite3.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include "get.h"

int SocketConnected(int sock);
#endif
