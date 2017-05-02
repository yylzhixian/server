#include "db.h"

/*
  function:数据库查询后的回调函数
  parameter:结果字符串，字段个数，字段值的数组，字段名的数组
  return:0
*/
static int callback(void *data, int argc, char **argv, char **azColName) {
  int i;
  for (i = 0; i < argc; i++) {
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}

/*
  function:数据库查询后的回调函数
  parameter:结果字符串，字段个数，字段值的数组，字段名的数组
  return:0
*/
static int incallback(void *data, int argc, char **argv, char **azColName) {
  return 0;
}
/*
  function:检查用户名是否存在
  parameter:结果字符串，字段个数，字段值的数组，字段名的数组
  return:0
*/
static int ccallback(void *data, int argc, char **argv, char **azColName) {
  *((int *)data) = argc;
  return 0;
}

/*
  function:根据token检查用户是否存在
  parameter:结果字符串，字段个数，字段值的数组，字段名的数组
  return:0
*/
static int cuallback(void *data, int argc, char **argv, char **azColName) {
  int i;
  ((pUser)data)->id = 1;
  for (i = 0; i < argc; i++) {
    if (strcmp(azColName[i], "name") == 0) {
      strcpy(((pUser)data)->name, argv[i]);
    }
  }
  return 0;
}
/*
  function:登录时数据库查询后的回调函数
  parameter:结果字符串，字段个数，字段值的数组，字段名的数组
  return:0
*/
static int lcallback(void *data, int argc, char **argv, char **azColName) {
  int i;
  int j;
  ((pUser)data)->id = 0;
  for (i = 0; i < argc; i++) {
    if (strcmp(azColName[i], "state") == 0 && strcmp(argv[i], "1") == 0) {
      for (j = 0; j < argc; j++) {
        if (strcmp(azColName[j], "token") == 0) {
          strcpy(((pUser)data)->token, argv[j]);
          ((pUser)data)->id = 1;
          break;
        }
      }
      break;
    }
  }
  return 0;
}

/*
  function:检查该用户是否为管理员的回调函数
  parameter:结果字符串，字段个数，字段值的数组，字段名的数组
  return:0
*/
static int iacallback(void *data, int argc, char **argv, char **azColName) {
  int i;
  ((pUser)data)->id = 0;
  for (i = 0; i < argc; i++) {
    if (strcmp(azColName[i], "id") == 0 && strcmp(argv[i], "1") == 0) {
      ((pUser)data)->id = 1;
      break;
    }
  }
  return 0;
}

/*
  function:管理员获取所有用户的回调函数
  parameter:结果字符串，字段个数，字段值的数组，字段名的数组
  return:0
*/
static int gucallback(void *data, int argc, char **argv, char **azColName) {
  int i;
  for (i = 0; i < argc; i++) {
    // printf("%s=%s\n", azColName[i], argv[i]);
    if (strcmp(azColName[i], "password") != 0 &&
        strcmp(azColName[i], "token") != 0) {
      strcat((char *)data, argv[i]);
      strcat((char *)data, "|");
    }
  }
  strcat((char *)data, ";");
  return 0;
}
/*
  function:获取当前日期时间字符串
  parameter:无
  return:日期时间字符串
*/
void getdate(char *token) {
  time_t now;
  struct tm *tm_now;
  time(&now);
  tm_now = localtime(&now);
  sprintf(token, "%d%d%d%d%d%d", tm_now->tm_year + 1900, tm_now->tm_mon + 1,
          tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
}

/*
  function:创建用户数据表
  parameter:数据库连接指针
  return:成功1,失败0
*/
int initDb(sqlite3 *db) {
  int rc;
  char *sql;
  char *errmsg;

  if (!sqlite3_open("home.db", &db)) {
    sql = "CREATE TABLE user("
          "id INTEGER PRIMARY KEY autoincrement NOT NULL,"
          "name   char(15)    NOT NULL,"
          "password char(20)     NOT NULL,"
          "idcard  CHAR(18) NOT NULL,"
          "state   CHAR(1) NOT NULL,"
          "token   char(20) NOT NULL );";
    rc = sqlite3_exec(db, sql, NULL, 0, &errmsg);
    if (rc != SQLITE_OK) {
      printf("SQL error: %s\n", errmsg);
      sqlite3_free(errmsg);
      return 0;
    } else {
      return 1;
    }
  }
  return 0;
}

/*
  function:检查用户名是否已存在
  parameter:数据库连接指针，用户结构体指针
  return:不存在1,存在0
*/
int checkName(sqlite3 *db, pUser user) {
  int rc;
  int count = 0;
  char sql[300];
  char *errmsg;

  if (!sqlite3_open("home.db", &db)) {
    strcpy(sql, "select * from user where name='");
    strcat(sql, user->name);
    strcat(sql, "'");
    rc = sqlite3_exec(db, sql, ccallback, (void *)&count, &errmsg);
    if (rc != SQLITE_OK) {
      printf("SQL error: %s\n", errmsg);
      sqlite3_free(errmsg);
      return 0;
    } else {
      if (count == 0) {
        return 1;
      }
    }
  }
  return 0;
}

/*
  function:新增用户
  parameter:数据库连接指针，用户结构体指针
  return:成功1,失败0
*/
int insertUser(sqlite3 *db, pUser user) {
  int rc;
  char sql[300];
  char token[20];
  char *errmsg;

  if (!sqlite3_open("home.db", &db)) {
    getdate(token);
    strcpy(sql, "INSERT INTO user (name,password,idcard,state,token) ");
    strcat(sql, "VALUES ('");
    strcat(sql, user->name);
    strcat(sql, "','");
    strcat(sql, user->password);
    strcat(sql, "','");
    strcat(sql, user->idcard);
    strcat(sql, "','0','");
    strcat(sql, token);
    strcat(sql, "');");
    rc = sqlite3_exec(db, sql, incallback, 0, &errmsg);
    if (rc != SQLITE_OK) {
      printf("SQL error: %s\n", errmsg);
      sqlite3_free(errmsg);
      return 0;
    } else {
      return 1;
    }
  }
  return 0;
}

/*
  function:用户登录
  parameter:数据库连接指针，用户结构体指针
  return:成功1,失败0
*/
int login(sqlite3 *db, pUser user) {
  int rc;
  char sql[300];
  char *errmsg;

  if (!sqlite3_open("home.db", &db)) {
    strcpy(sql, "select * from user where name='");
    strcat(sql, user->name);
    strcat(sql, "' and password='");
    strcat(sql, user->password);
    strcat(sql, "';");
    user->id = 0;
    rc = sqlite3_exec(db, sql, lcallback, (void *)user, &errmsg);
    if (rc != SQLITE_OK) {
      printf("SQL error: %s\n", errmsg);
      sqlite3_free(errmsg);
      return 0;
    } else {
      return user->id;
    }
  }
  return 0;
}

/*
  function:检查该用户是否为管理员
  parameter:数据库连接指针，用户结构体指针
  return:成功1,失败0
*/
int isAdmin(sqlite3 *db, pUser user) {
  int rc;
  char sql[300];
  char *errmsg;

  if (!sqlite3_open("home.db", &db)) {
    strcpy(sql, "select * from user where token='");
    strcat(sql, user->token);
    strcat(sql, "';");
    user->id = 0;
    rc = sqlite3_exec(db, sql, iacallback, (void *)user, &errmsg);
    if (rc != SQLITE_OK) {
      printf("SQL error: %s\n", errmsg);
      sqlite3_free(errmsg);
      return 0;
    } else {
      return user->id;
    }
  }
  return 0;
}

/*
  function:审核用户通过
  parameter:数据库连接指针，用户结构体指针
  return:成功1,失败0
*/
int setUserOn(sqlite3 *db, pUser user) {
  int rc;
  char sql[300];
  char *errmsg;
  char id[5];

  if (!sqlite3_open("home.db", &db)) {
    sprintf(id, "%d", user->id);
    strcpy(sql, "update user set state=1 where id=");
    strcat(sql, id);
    strcat(sql, ";");
    rc = sqlite3_exec(db, sql, NULL, 0, &errmsg);
    if (rc != SQLITE_OK) {
      printf("SQL error: %s\n", errmsg);
      sqlite3_free(errmsg);
      return 0;
    } else {
      return 1;
    }
  }
  return 0;
}

/*
  function:审核用户不通过
  parameter:数据库连接指针，用户结构体指针
  return:成功1,失败0
*/
int setUserOff(sqlite3 *db, pUser user) {
  int rc;
  char sql[300];
  char *errmsg;
  char id[5];

  if (!sqlite3_open("home.db", &db)) {
    sprintf(id, "%d", user->id);
    strcpy(sql, "update user set state=2 where id=");
    strcat(sql, id);
    strcat(sql, ";");
    rc = sqlite3_exec(db, sql, NULL, 0, &errmsg);
    if (rc != SQLITE_OK) {
      printf("SQL error: %s\n", errmsg);
      sqlite3_free(errmsg);
      return 0;
    } else {
      return 1;
    }
  }
  return 0;
}

/*
  function:管理员获取所有用户列表
  parameter:数据库连接指针，用户列表字符串指针
  return:成功1,失败0
*/
int getAllUsers(sqlite3 *db, char *msg) {
  int rc;
  char sql[300];
  char *errmsg;

  if (!sqlite3_open("home.db", &db)) {
    strcpy(sql, "select * from user;");
    rc = sqlite3_exec(db, sql, gucallback, (void *)msg, &errmsg);
    if (rc != SQLITE_OK) {
      printf("SQL error: %s\n", errmsg);
      sqlite3_free(errmsg);
      return 0;
    } else {
      return 1;
    }
  }
  return 0;
}

/*
  function:根据token检查用户是否已存在
  parameter:数据连接指针，用户结构体指针
  return:不存在0,存在1
*/
int checkUser(sqlite3 *db, pUser user) {
  int rc;
  int count = 0;
  char sql[300];
  char *errmsg;

  if (!sqlite3_open("home.db", &db)) {
    strcpy(sql, "select * from user where token='");
    strcat(sql, user->token);
    strcat(sql, "';");
    user->id = 0;
    rc = sqlite3_exec(db, sql, cuallback, (void *)user, &errmsg);
    if (rc != SQLITE_OK) {
      printf("SQL error: %s\n", errmsg);
      sqlite3_free(errmsg);
      return 0;
    } else {
      if (user->id == 1) {
        return 1;
      }
    }
  }
  return 0;
}

/*
  function:修改密码
  parameter:数据库连接指针，用户结构体指针
  return:成功1,失败0
*/
int modifyPassword(sqlite3 *db, pUser user) {
  int rc;
  char sql[300];
  char *errmsg;

  if (!sqlite3_open("home.db", &db)) {
    strcpy(sql, "update user set password=");
    strcpy(sql, user->password);
    strcpy(sql, " where token=");
    strcat(sql, user->token);
    strcat(sql, ";");
    rc = sqlite3_exec(db, sql, NULL, 0, &errmsg);
    if (rc != SQLITE_OK) {
      printf("SQL error: %s\n", errmsg);
      sqlite3_free(errmsg);
      return 0;
    } else {
      return 1;
    }
  }
  return 0;
}
