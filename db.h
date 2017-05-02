#ifndef _DB_H
#define _DB_H
#include <stdio.h>
#include <sqlite3.h>
#include "global.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int callback(void *data, int argc, char **argv, char **azColName);
static int ccallback(void *data, int argc, char **argv, char **azColName);
static int lcallback(void *data, int argc, char **argv, char **azColName);
static int iacallback(void *data, int argc, char **argv, char **azColName);
static int gucallback(void *data, int argc, char **argv, char **azColName);
void getdate(char *token);
int initDb(sqlite3 *db);
int checkName(sqlite3 *db, pUser user);
int insertUser(sqlite3 *db, pUser user);
int login(sqlite3 *db, pUser user);
int isAdmin(sqlite3 *db, pUser user);
int setUserOn(sqlite3 *db, pUser user);
int setUserOff(sqlite3 *db, pUser user);
int getAllUsers(sqlite3 *db, char *msg);
int checkUser(sqlite3 *db, pUser user);
int modifyPassword(sqlite3 *db, pUser user);

#endif
