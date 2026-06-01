#include "PreparedStmt.h"

PreparedStmt::PreparedStmt(MYSQL *conn, const char *sql)
{
    stmt = mysql_stmt_init(conn);
    if (!stmt)
    {
        throw std::runtime_error("Failed to initialize statement");
    }

    if (mysql_stmt_prepare(stmt, sql, static_cast<unsigned long>(strlen(sql))) != 0)
    {
        mysql_stmt_close(stmt);
        throw std::runtime_error("Failed to prepare statement");
    }
}

PreparedStmt::~PreparedStmt()
{
    if (stmt)
    {
        mysql_stmt_close(stmt);
        stmt = nullptr;
    }
}

bool PreparedStmt::bind_param(MYSQL_BIND *_input)
{
    return mysql_stmt_bind_param(stmt, _input) == 0;
}
bool PreparedStmt::execute()
{
    return mysql_stmt_execute(stmt) == 0;
}
bool PreparedStmt::bind_result(MYSQL_BIND *_output)
{
    return mysql_stmt_bind_result(stmt, _output) == 0;
}

bool PreparedStmt::fetch_row() // 获取一行数据
{
    return mysql_stmt_fetch(stmt) == 0;
}
