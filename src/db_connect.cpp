
#include "db_connect.h"

DBconnect::DBconnect(const DBconfig &_config)
{
    conn = mysql_init(nullptr);
    if (!conn)
    {
        throw std::runtime_error("mysql_init failed");
    }

    if (!mysql_real_connect(conn, _config.host.c_str(), _config.user.c_str(), _config.password.c_str(), _config.database.c_str(), _config.port, _config.__unix_socket, _config.__client_flag))
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

MYSQL_RES *DBconnect::query(const std::string &_sql)
{
    if (mysql_ping(conn)) // 连接失败
    {
        throw std::runtime_error(mysql_error(conn));
    }
    if (mysql_query(conn, _sql.c_str()) != 0)
    {
        throw std::runtime_error(mysql_error(conn));
    }
    return mysql_store_result(conn);
}

bool DBconnect::execute(const std::string &_sql)
{
    if (mysql_ping(conn)) // 连接失败
    {
        throw std::runtime_error(mysql_error(conn));
    }
    return !mysql_query(conn, _sql.c_str());
}
