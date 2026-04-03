#pragma once

#include <iostream>
#include <mysql.h>
#include <string>
#include <sstream>

struct DBconfig
{
    const char *host;
    const char *user;
    const char *password;
    const char *database;
    size_t port;
    const char *__unix_socket = nullptr;
    size_t __client_flag = 0;
};

class DBconnect
{
public:
    DBconnect();
    DBconnect(const DBconfig &config);
    ~DBconnect();

    MYSQL_RES *query(const std::string &sql);
    bool execute(const std::string &sql);

private:
    MYSQL *conn;
};
