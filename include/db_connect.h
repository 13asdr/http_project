#pragma once

#include <mysql.h>

#include <string>
#include <stdexcept>

#include "config.h"

class DbConnect
{
public:
    explicit DbConnect(const DbConfig &_config);
    ~DbConnect();

    MYSQL_RES *query(const std::string &_sql);
    bool execute(const std::string &_sql);

private:
    MYSQL *conn;
};
