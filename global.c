#include "global.h"

// 声明一个发布链表和一个订阅链表
pNode pHead;
ssNode ssHead;
sqlite3 *db;

int SocketConnected(int sock) {
  if (sock <= 0)
    return 0;
  struct tcp_info info;
  int len = sizeof(info);
  getsockopt(sock, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);
  if ((info.tcpi_state == TCP_ESTABLISHED)) {
    return 1;
  } else {
    return 0;
  }
}
