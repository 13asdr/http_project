#pragma once

#include <iostream>
#include <mysql.h>
#include <string>
#include <sstream>
#include "config.h"


class DBconnect
{
public:
    DBconnect(const DBconfig &config);
    ~DBconnect();

    MYSQL_RES *query(const std::string &sql);
    bool execute(const std::string &sql);

private:
    MYSQL *conn;
};
