#include "publish.h"

/*
  function:创建一个发布链表
  parameter:无
  return:表头
*/
pNode createPList() {
  pNode head = (pNode)malloc(sizeof(struct publish));
  head->next = NULL;
  return head;
}

/*
  function:打印出发布链表中的所有记录
  parameter:发布列表头
  return:无
*/
void printAllPub(pNode head) {
  pNode p = head->next;
  while (p) {
    printf("话题：%s，内容：%s\n", p->name, p->content);
    p = p->next;
  }
}
/*
  function:为指定主题的所有设备发推送
  parameter:订阅列表头，主题名，推送的内容
  return:无
*/
void sendPropel(ssNode head, char *n, char *c) {
  // 得到订阅指定话题的设备链表头
  sdNode sdHead = findsdHead(head, n);
  // 若为空则函数执行结束
  if (!sdHead) {
    return;
  }
  sdNode p = sdHead->next;
  char str[30];
  // 拼接好消息字符串
  strcpy(str, "s,");
  strcat(str, n);
  strcat(str, ",");
  strcat(str, c);
  strcat(str, ",");
  // 循环设备链表，发送消息
  while (p) {
    // 根据设备的socket连接号发送消息
    if (SocketConnected(p->sockid)) {
      send(p->sockid, str, strlen(str), 0);
      printf("发布推送，设备：%d，主题：%s，内容：%s\n", p->sockid, n, c);
    }
    p = p->next;
  }
}

/*
  function:把实时数据发送给云端
  parameter:话题名，话题内容
  return:NULL
*/
void dataCollection(char *n, char *c) {
  if (strcmp(n, "white") == 0) {
    if (strcmp(c, "on") == 0) {
      sendHttp("S001", "1");
    } else {
      sendHttp("S001", "0");
    }
  } else if (strcmp(n, "color") == 0) {
    if (strcmp(c, "on") == 0) {
      sendHttp("S002", "1");
    } else {
      sendHttp("S002", "0");
    }
  } else if (strcmp(n, "fan") == 0) {
    if (strcmp(c, "on") == 0) {
      sendHttp("S003", "1");
    } else {
      sendHttp("S003", "0");
    }
  } else if (strcmp(n, "buzzer") == 0) {
    if (strcmp(c, "on") == 0) {
      sendHttp("S004", "1");
    } else {
      sendHttp("S004", "0");
    }
  } else if (strcmp(n, "temp") == 0) {
    sendHttp("S005", c);
  } else if (strcmp(n, "hum") == 0) {
    sendHttp("S006", c);
  } else if (strcmp(n, "infrared") == 0) {
    sendHttp("S007", c);
  } else if (strcmp(n, "curtain") == 0) {
    if (strcmp(c, "on") == 0) {
      sendHttp("S008", "1");
    } else {
      sendHttp("S008", "0");
    }
  }
}
/*
  function:发布一个话题，若存在则更新内容，不存在则新建，并且发推送信息
  parameter:发布的表头，话题名，话题内容
  return:NULL
*/
void pNodePush(pNode head, ssNode shead, char *n, char *c) {
  pNode p = head->next;
  pNode pre = head;
  pNode node = NULL;

  // 实时数据采集
  dataCollection(n, c);
  // 循环整个发布链表
  while (p) {
    // 若该话题存在则更新内容，不存在则新建节点
    if (strcmp(p->name, n) == 0) {
      strcpy(p->content, c);
      printf("更新话题：%s，内容：%s\n", n, c);
      // 更新话题后会给订阅了该话题的所有设备发推送
      sendPropel(shead, n, c);
      return;
    }
    pre = p;
    p = p->next;
  }
  // 新建话题节点
  node = (pNode)malloc(sizeof(struct publish));
  strcpy(node->name, n);
  strcpy(node->content, c);
  node->next = NULL;
  pre->next = node;
  printf("发布话题：%s，内容：%s\n", n, c);
  // 新话题发布后，也要给订阅了该话题的所有设备发推送
  sendPropel(shead, n, c);
}
