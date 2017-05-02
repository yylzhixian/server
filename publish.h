#ifndef _PUBLISH_H
#define _PUBLISH_H
#include <string.h>
#include <malloc.h>
#include "global.h"
#include "subscribe.h"

// 声明一个发布链表和一个订阅链表
extern pNode pHead;
extern ssNode ssHead;

pNode createPList();
void printAllPub(pNode head);
void sendPropel(ssNode head, char *n, char *c);
void pNodePush(pNode head, ssNode shead, char *n, char *c);
#endif
