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
    std::string escape_string(const std::string &_input);

    inline MYSQL *get_conn() const
    {
        return conn;
    }

private:
    MYSQL *conn;
};
