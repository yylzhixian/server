#include "extention.h"

extern pNode pHead;
extern ssNode ssHead;

int main(int argc, char const *argv[]) {
  int para[2];
  // 新客户端连接成功时的标识号
  int connect;
  // 新线程的标识号
  pthread_t pid;
  // 初始化socket对象
  int sk = init("192.168.1.100", 8888);
  // 创建发布链表
  pHead = createPList();
  // 创建订阅链表
  ssHead = createSList();
  // 循环等待新客户端连接
  while (1) {
    // 新客户端连接
    connect = accept(sk, NULL, NULL);
    if (connect == -1) {
      perror("connect failed!");
      continue;
    }
    printf("connect a client：%d\n", connect);
    // 为新客户端创建线程，并把服务器的sockid和客户端自身的sockid传递过去
    para[0] = sk;
    para[1] = connect;
    pthread_create(&pid, NULL, while_thread, para);
  }
  close(sk);
  return 0;
}
