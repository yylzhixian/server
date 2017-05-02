#ifndef _SUBSCRIBE_H
#define _SUBSCRIBE_H
#include <string.h>
#include <malloc.h>
#include "global.h"

// 声明一个发布链表和一个订阅链表
extern pNode pHead;
extern ssNode ssHead;

ssNode createSList();
sdNode createSDList();
void printAllSubDev(sdNode head);
void printAllSub(ssNode head);
void findsdNode(sdNode head, int sk);
void sendPro(pNode pHead, char *n, int sk,char c);
sdNode findsdHead(ssNode head, char *n);
void findSubNode(ssNode head, char *n, int sk);
void sNodePush(ssNode head, pNode phead, char *n, int sk,char c);
void deleteAllDevice(ssNode head, int sk);

#endif
