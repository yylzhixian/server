#include "get.h"

void getHttp(char *m) {
  char msg[256];
  char *ip = "192.168.1.102";
  int port = 8080;
  int sockfd;
  struct sockaddr_in servaddr;
  char str[4096];
  char slen[128];
  char recvmsg[128];

  strcpy(msg, (char *)m);
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("创建网络连接失败,本线程即将终止---socket error!\n");
    return;
  }
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);
  servaddr.sin_addr.s_addr = inet_addr(ip);

  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr)) <
      0) {
    printf("连接到服务器失败,connect error!\n");
    return;
  }
  printf("与远端建立了连接\n");
  memset(slen, 0, sizeof(slen));
  sprintf(slen, "%ld", strlen(msg));
  sprintf(str, "GET /SmartHome/addLog_upload.do?%s HTTP/1.1\r\nHOST: "
               "%s:%d\r\nAccept: */*\r\n\r\n",
          msg, ip, port);
  write(sockfd, str, strlen(str));
  close(sockfd);
}

void sendHttp(char *type, char *value) {
  char msg[512];

  strcpy(msg, "dev_id=");
  strcat(msg, type);
  strcat(msg, "&val=");
  strcat(msg, value);
  getHttp(msg);
}
