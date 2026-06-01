#pragma once

#include <iostream>
#include <mysql.h>
#include <initializer_list>
#include <optional>
#include <functional>
#include <vector>


class PreparedStmt
{
public:
    PreparedStmt(MYSQL *conn, const char *sql);

    ~PreparedStmt();

    bool bind_param(MYSQL_BIND *_input);
    bool execute();
    bool bind_result(MYSQL_BIND *_output);
    bool fetch_row(); //huo

    template <typename T>
    std::optional<T> fetch_one(std::function<T(MYSQL_STMT *)> extractor)
    {
        if (mysql_stmt_fetch(stmt) != 0)
        {
            return std::nullopt;
        }
        return extractor(stmt);
    }

    template <typename T>
    std::vector<T> fetch_all(std::function<T(MYSQL_STMT *)> extractor)
    {
        std::vector<T> results;
        while (mysql_stmt_fetch(stmt) == 0)
        {
            results.push_back(extractor(stmt));
        }
        return results;
    }

private:
    MYSQL_STMT *stmt;
};
