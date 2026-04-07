
#include "DBconnect.h"

DBconnect::DBconnect(const DBconfig &config)
{
    conn = mysql_init(nullptr);
    if (!conn)
    {
        throw std::runtime_error("mysql_init failed");
    }

    if (!mysql_real_connect(conn, config.host.c_str(), config.user.c_str(), config.password.c_str(), config.database.c_str(), config.port, config.__unix_socket, config.__client_flag))
    {
        throw std::runtime_error(mysql_error(conn));
    }
    mysql_set_character_set(conn, "utf8mb4");
}

DBconnect::~DBconnect()
{
    if (conn)
    {
        mysql_close(conn);
    }
}

MYSQL_RES *DBconnect::query(const std::string &sql)
{
    if (mysql_ping(conn)) // 连接失败
    {
        throw std::runtime_error(mysql_error(conn));
    }
    if (mysql_query(conn, sql.c_str()) != 0)
    {
        throw std::runtime_error(mysql_error(conn));
    }
    return mysql_store_result(conn);
}

bool DBconnect::execute(const std::string &sql)
{
    if (mysql_ping(conn)) // 连接失败
    {
        throw std::runtime_error(mysql_error(conn));
    }
    return !mysql_query(conn, sql.c_str());
}
