#include "subscribe.h"
/*
  function:创建一个订阅列表
  parameter:无
  return:表头
*/
ssNode createSList() {
  ssNode head = (ssNode)malloc(sizeof(struct subscribe));
  head->next = NULL;
  return head;
}
/*
  function:创建一个设备列表
  parameter:无
  return:表头
*/
sdNode createSDList() {
  sdNode head = (sdNode)malloc(sizeof(struct subDevList));
  head->next = NULL;
  return head;
}

/*
  function:打印出指定设备链表中的所有设备号
  parameter:设备链表头
  return:无
*/
void printAllSubDev(sdNode head) {
  sdNode p = head->next;
  if (head->next == NULL) {
    printf("%s\n", "设备链表是空的");
  }
  while (p) {
    printf("\t设备号：%d\n", p->sockid);
    p = p->next;
  }
}

/*
  function:打印出订阅链表中的所有记录
  parameter:订阅链表头
  return:无
*/
void printAllSub(ssNode head) {
  ssNode p = head->next;
  if (head->next == NULL) {
    printf("%s\n", "订阅链表是空的");
  }
  while (p) {
    printf("订阅话题：%s\n", p->name);
    printAllSubDev(p->sdl);
    p = p->next;
  }
}

/*
  function:判断编号为sk的设备是否在列表中，不存在则新建
  parameter:设备列表表头，编号
  return:无
*/
void findsdNode(sdNode head, int sk) {
  sdNode pre = head;
  sdNode p = head->next;
  sdNode node = NULL;
  while (p) {
    if (p->sockid == sk) {
      return;
    }
    pre = p;
    p = p->next;
  }
  node = (sdNode)malloc(sizeof(struct subDevList));
  node->sockid = sk;
  node->next = NULL;
  pre->next = node;
}
/*
  function:根据订阅的主题名，查找该主题下的所有设备
  parameter:订阅列表表头，主题名
  return:设备列表表头
*/
sdNode findsdHead(ssNode head, char *n) {
  ssNode p = head->next;
  while (p) {
    if (strcmp(p->name, n) == 0) {
      return p->sdl;
    }
    p = p->next;
  }
  return NULL;
}

/*
  function:订阅成功后,在话题列表中查找，若存在则推送订阅的话题内容
  parameter:订阅列表表头，主题名，该设备id
  return:无
*/
void sendPro(pNode pHead, char *n, int sk, char c) {
  pNode p = pHead->next;
  char str[30];

  while (p) {
    if (strcmp(p->name, n) == 0) {
      // 拼接好消息字符串
      strcpy(str, "s,");
      strcat(str, n);
      strcat(str, ",");
      strcat(str, p->content);
      strcat(str, ",");
      send(sk, str, strlen(str), 0);
      break;
    }
  }
}
/*
  function:判断该主题是否存在于订阅列表中,不存在则创建
  parameter:订阅列表表头，主题名
  return:该主题所在节点
*/
void findSubNode(ssNode head, char *n, int sk) {
  ssNode pre = head;
  ssNode p = head->next;
  ssNode node = NULL;
  sdNode sdh = NULL;
  sdNode sdn = NULL;
  while (p) {
    if (strcmp(p->name, n) == 0) {
      findsdNode(p->sdl, sk);
      return;
    }
    pre = p;
    p = p->next;
  }
  node = (ssNode)malloc(sizeof(struct subscribe));
  sdh = createSDList();
  sdn = (sdNode)malloc(sizeof(struct subDevList));
  sdn->sockid = sk;
  sdn->next = NULL;
  sdh->next = sdn;
  strcpy(node->name, n);
  node->sdl = sdh;
  node->next = NULL;
  pre->next = node;
}
/*
  function:往订阅列表中插入一个主题节点，以及往该主题设备列表中插入一个设备
  parameter:订阅列表表头，主题名，设备sk
  return:无
*/
void sNodePush(ssNode head, pNode phead, char *n, int sk, char c) {
  findSubNode(head, n, sk);
  sendPro(phead, n, sk, c);
  printf("设备：%d,订阅话题：%s\n", sk, n);
}
/*
  function:将指定设备从订阅表中完全删除，以为可能会有多个
  parameter:订阅列表表头，设备sk
  return:无
*/
void deleteAllDevice(ssNode head, int sk) {
  // 订阅表表头
  ssNode p = head->next;
  // 设备表当前节点
  sdNode q = NULL;
  // 设备表当前节点的前一个节点
  sdNode pre = NULL;
  // 循环遍历订阅表
  while (p) {
    pre = p->sdl;
    q = p->sdl->next;
    // 循环遍历每个订阅节点中的每个设备节点
    while (q) {
      // 如果找到则删除该节点
      if (q->sockid == sk) {
        pre->next = q->next;
        free(q);
        q = pre->next;
      } else {
        pre = q;
        q = q->next;
      }
    }
    p = p->next;
  }
}
