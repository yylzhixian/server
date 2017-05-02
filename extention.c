#include "extention.h"

/*
  function:初始化socket通信
  parameter：绑定的ip和端口号
  return:初始化之后的socket标识号
*/
int init(char *ip, int port) {
  // create socket
  int sk = socket(AF_INET, SOCK_STREAM, 0);
  if (sk == -1) {
    perror("create socket failed!");
    exit(-1);
  }
  // bind addr
  int on = 1;
  // 声明地址结构体
  struct sockaddr_in addr;
  // 初始化结构体
  memset(&addr, 0, sizeof(struct sockaddr_in));
  // 地址类型为ipv4
  addr.sin_family = AF_INET;
  // 设置端口号
  addr.sin_port = htons(port);
  // 绑定ip
  addr.sin_addr.s_addr = inet_addr(ip);
  setsockopt(sk, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
  if (-1 == bind(sk, (struct sockaddr *)&addr, sizeof(addr))) {
    perror("bind addr failed!");
    close(sk);
    exit(-1);
  }
  // 设置为监听状态，客户端在线数上线为128
  if (-1 == listen(sk, 128)) {
    perror("listen failed!");
    close(sk);
    exit(-1);
  }
  printf("%s\n", "socket init success!");
  return sk;
}

/*
  function:将接收的字符串转换为msg结构体
  parameter:字符串指针，msg结构体指针
  return:无
*/
void clientMsg(char *str) {
  char temp[22];
  char *ch = str;
  int counter = 0;
  int symbol = 0;
  // 初始化temp缓存数组
  memset(temp, 0, sizeof(temp));
  // 用ch字符指针遍历str字符串
  while ((*ch) != '\0' && symbol < 4) {
    // 遇到“,”时则根据symbol计数器判断出当前小段字符串的含义
    if ((*ch) == ',') {
      strcpy(cmsg[symbol], temp);
      memset(temp, 0, sizeof(temp));
      counter = 0;
      ++symbol;
    } else {
      *(temp + counter) = *ch;
      ++counter;
    }
    ++ch;
  }
}
/*
  function:将接收的字符串转换为msg结构体
  parameter:字符串指针，msg结构体指针
  return:无
*/
void handleMsg(char *str, pMsg msg) {
  char temp[10];
  char *ch = str;
  int counter = 0;
  int symbol = 1;
  // 初始化temp缓存数组
  memset(temp, 0, sizeof(temp));
  // 用ch字符指针遍历str字符串
  while ((*ch) != '\0' && symbol <= 4) {
    // 遇到“,”时则根据symbol计数器判断出当前小段字符串的含义
    if ((*ch) == ',') {
      switch (symbol) {
      // symbol是1时，temp的内容为设备类型
      case 1:
        msg->type = temp[0];
        break;
      // symbol是2时，temp的内容为操作类型
      case 2:
        msg->action = temp[0];
        break;
      // symbol是3时，temp的内容为主题名称
      case 3:
        strcpy(msg->name, temp);
        break;
      // symbol是4时，temp的内容为主题的内容
      case 4:
        strcpy(msg->content, temp);
        break;
      }
      memset(temp, 0, sizeof(temp));
      counter = 0;
      ++symbol;
    } else {
      *(temp + counter) = *ch;
      ++counter;
    }
    ++ch;
  }
}
/*
  function:根据客户端类型处理操作
  parameter:设备类型，客户端的socket连接号，操作类型，主题名称，数据
  return:无
*/
void handleClient(char type, int sk, char action, char *title, char *content) {
  // 判断设备类型，a（传感器），b（控制器），c（用户端）
  switch (type) {
  case 'a':
    // 传感器只能发布主题
    if ('p' == action) {
      pNodePush(pHead, ssHead, title, content);
    }
    break;
  case 'b':
    // 控制器只能订阅主题
    if ('s' == action) {
      sNodePush(ssHead, pHead, title, sk, 'b');
    }
    break;
  case 'c':
    // 用户端两者都可以
    switch (action) {
    case 'p':
      pNodePush(pHead, ssHead, title, content);
      break;
    case 's':
      sNodePush(ssHead, pHead, title, sk, 'c');
      break;
    }
    break;
  }
}
/*
  function:获取当前日期时间字符串
  parameter:无
  return:日期时间字符串
*/
void gettime(char *token) {
  time_t now;
  struct tm *tm_now;
  time(&now);
  tm_now = localtime(&now);
  sprintf(token, "[%d-%d-%d %d:%d:%d]", tm_now->tm_year + 1900,
          tm_now->tm_mon + 1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min,
          tm_now->tm_sec);
}
/*
  function:写入日志
  parameter:操作人姓名、操作类型、操作结果、操作时间
  return:无
*/
void writeRecord(char *name, char *action, char *result) {
  char msg[200];
  char t[50];
  gettime(t);
  strcpy(msg, t);
  strcat(msg, ",");
  strcat(msg, name);
  strcat(msg, ",");
  strcat(msg, action);
  strcat(msg, ",");
  strcat(msg, result);
  strcat(msg, "\n");
  FILE *fd = fopen("record.txt", "a+");
  if (fd) {
    fseek(fd, 0, SEEK_SET);
    fwrite(msg, strlen(msg), 1, fd);
    fclose(fd);
  }
}
/*
  function:回复客户端信息
  parameter:客户端的socket连接号、操作类型、结果代码、具体信息
  return:无
*/
void sendToClient(int sk, char type, char result, char *msg) {
  char *data;
  if (type == 'u' && result == '1') {
    data = (char *)malloc(sizeof(char) * 1000);
  } else {
    data = (char *)malloc(sizeof(char) * 200);
  }
  data[0] = type;
  data[1] = ',';
  data[2] = result;
  data[3] = ',';
  data[4] = '\0';
  strcat(data, msg);
  strcat(data, ",");
  send(sk, data, strlen(data), 0);
}

/*
  function:新线程的调用执行函数
  parameter:新客户端的socket连接号
  return:无
*/
void *while_thread(void *argv) {
  int sk = ((int *)argv)[0];
  // 获得该客户端的连接标识号
  int connect = ((int *)argv)[1];
  // 数据接收状态标识
  int recv_state;
  // 缓存空间
  char buff[256];
  // 创建msg消息结构体
  pMsg msg = (pMsg)malloc(sizeof(struct msg));
  // 循环等待接收客户端发送的数据
  while (1) {
    // 在每次接收数据之前初始化缓存空间
    memset(buff, 0, sizeof(buff));
    // 接收数据
    recv_state = recv(connect, buff, sizeof(buff), 0);
    if (-1 == recv_state) {
      perror("recv failed!");
      deleteAllDevice(ssHead, connect);
      pthread_exit(NULL);
    } else if (0 == recv_state) {
      // 删除该设备在订阅列表中的所有记录
      deleteAllDevice(ssHead, connect);
      printf("%s\n", "client quit!");
      pthread_exit(NULL);
    } else {
      // 客户端发送一个字母q时关闭服务器
      if (buff[0] == 'q') {
        printf("%s %d\n", "server was shutdown by client", connect);
        close(sk);
        exit(0);
      }
      // 客户端发送一个字母p时打印出发布列表，s打印出订阅列表
      if (buff[0] == 'p') {
        printAllPub(pHead);
      } else if (buff[0] == 's') {
        printAllSub(ssHead);
      } else {
        // 去掉所接收字符串尾部的回车符号
        if (buff[strlen(buff) - 2] == 13) {
          buff[strlen(buff) - 2] = '\0';
        }
        printf("%s\n", buff);
        // buff[0]=='c'时为手机客户端
        if (buff[0] == 'a' || buff[0] == 'b') {
          // 预处理字符串
          handleMsg(buff, msg);
          // 根据消息执行操作
          handleClient(msg->type, connect, msg->action, msg->name,
                       msg->content);
        } else if (buff[0] == 'c') {
          // 创建用户结构体
          pUser user = (pUser)malloc(sizeof(struct user));
          switch (buff[2]) {
          // 注册
          case 'r':
            clientMsg(buff);
            strcpy(user->name, cmsg[2]);
            strcpy(user->password, cmsg[3]);
            strcpy(user->idcard, cmsg[4]);
            if (checkName(db, user)) {
              if (insertUser(db, user)) {
                writeRecord(user->name, "注册", "注册成功");
                sendToClient(connect, 'r', '1', "注册成功");
              } else {
                writeRecord(user->name, "注册", "系统繁忙请稍候再试");
                sendToClient(connect, 'r', '0', "系统繁忙请稍候再试");
              }
            } else {
              writeRecord(user->name, "注册", "用户名已存在请重新输入");
              sendToClient(connect, 'r', '0', "用户名已存在请重新输入");
            }
            break;
          // 登录
          case 'l':
            clientMsg(buff);
            strcpy(user->name, cmsg[2]);
            strcpy(user->password, cmsg[3]);
            if (!checkName(db, user)) {
              if (login(db, user)) {
                writeRecord(user->name, "登录", "登录成功");
                sendToClient(connect, 'l', '1', user->token);
              } else {
                writeRecord(user->name, "登录", "密码错误或待管理员审核");
                sendToClient(connect, 'l', '0', "密码错误或待管理员审核");
              }
            } else {
              writeRecord(user->name, "登录", "用户不存在");
              sendToClient(connect, 'l', '0', "用户不存在");
            }
            break;
          // 管理员获得用户列表
          case 'u':
            clientMsg(buff);
            strcpy(user->token, cmsg[2]);
            if (isAdmin(db, user)) {
              char userslist[1000];
              strcpy(userslist, "");
              if (getAllUsers(db, userslist)) {
                writeRecord("admin", "获取用户列表", "获取成功");
                sendToClient(connect, 'u', '1', userslist);
              } else {
                writeRecord("admin", "获取用户列表", "系统繁忙");
                sendToClient(connect, 'u', '0', "系统繁忙");
              }
            } else {
              writeRecord("admin", "获取用户列表", "非法操作");
              sendToClient(connect, 'u', '0', "非法操作");
            }
            break;
          // 管理员授权用户
          case 't':
            clientMsg(buff);
            strcpy(user->token, cmsg[2]);
            user->id = atoi(cmsg[3]);
            if (isAdmin(db, user)) {
              if (setUserOn(db, user)) {
                strcpy(buff, "操作成功 uid=");
                strcat(buff, cmsg[3]);
                writeRecord(user->name, "管理员授权用户", buff);
                sendToClient(connect, 't', '1', "操作成功");
              } else {
                strcpy(buff, "系统繁忙 uid=");
                strcat(buff, cmsg[3]);
                writeRecord(user->name, "管理员授权用户", buff);
                sendToClient(connect, 't', '0', "系统繁忙");
              }
            } else {
              strcpy(buff, "非法操作 uid=");
              strcat(buff, cmsg[3]);
              writeRecord(user->name, "管理员授权用户", buff);
              sendToClient(connect, 't', '0', "非法操作");
            }
            break;
          // 管理员拒绝用户
          case 'b':
            clientMsg(buff);
            strcpy(user->token, cmsg[2]);
            user->id = atoi(cmsg[3]);
            if (isAdmin(db, user) && user->id != 1) {
              if (setUserOff(db, user)) {
                strcpy(buff, "操作成功 uid=");
                strcat(buff, cmsg[3]);
                writeRecord(user->name, "管理员拒绝用户", buff);
                sendToClient(connect, 'b', '1', "操作成功");
              } else {
                strcpy(buff, "系统繁忙 uid=");
                strcat(buff, cmsg[3]);
                writeRecord(user->name, "管理员拒绝用户", buff);
                sendToClient(connect, 'b', '0', "系统繁忙");
              }
            } else {
              strcpy(buff, "非法操作 uid=");
              strcat(buff, cmsg[3]);
              writeRecord(user->name, "管理员拒绝用户", buff);
              sendToClient(connect, 'b', '0', "非法操作");
            }
            break;
          // 修改密码
          case 'm':
            clientMsg(buff);
            strcpy(user->token, cmsg[2]);
            strcpy(user->password, cmsg[3]);
            if (checkUser(db, user)) {
              strcpy(buff, "修改密码成功：");
              strcat(buff, cmsg[3]);
              writeRecord(user->name, "修改密码", buff);
              sendToClient(connect, 'm', '0', "修改成功");
            } else {
              strcpy(buff, "修改密码失败：");
              strcat(buff, cmsg[3]);
              writeRecord(user->name, "修改密码", buff);
              sendToClient(connect, 'm', '1', "修改失败");
            }
            break;
          // 订阅话题
          case 's':
            clientMsg(buff);
            strcpy(user->token, cmsg[3]);
            if (checkUser(db, user)) {
              strcpy(buff, "订阅成功 话题：");
              strcat(buff, cmsg[2]);
              writeRecord(user->name, "订阅话题", buff);
              sNodePush(ssHead, pHead, cmsg[2], connect, 'c');
            } else {
              strcpy(buff, "请登录后操作 话题：");
              strcat(buff, cmsg[2]);
              writeRecord(user->name, "订阅话题", buff);
              sendToClient(connect, 's', '0', "请登录后操作");
            }
            break;
          // 发布话题
          case 'p':
            clientMsg(buff);
            strcpy(user->token, cmsg[4]);
            if (checkUser(db, user)) {
              strcpy(buff, "发布成功 话题：");
              strcat(buff, cmsg[2]);
              strcpy(buff, "值：");
              strcat(buff, cmsg[3]);
              writeRecord(user->name, "发布话题", buff);
              pNodePush(pHead, ssHead, cmsg[2], cmsg[3]);
            } else {
              strcpy(buff, "发布成功 话题：");
              strcat(buff, cmsg[2]);
              strcpy(buff, "值：");
              strcat(buff, cmsg[3]);
              writeRecord(user->name, "发布话题", buff);
              sendToClient(connect, 'p', '0', "请登录后操作");
            }
            break;
          }
        }
      }
    }
  }
}
