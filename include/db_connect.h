#pragma once

#include <iostream>
#include <mysql.h>
#include <string>
#include <sstream>
#include "config.h"


class DBconnect
{
public:
    DBconnect(const DBconfig &_config);
    ~DBconnect();

    MYSQL_RES *query(const std::string &_sql);
    bool execute(const std::string &_sql);

private:
    MYSQL *conn;
};
