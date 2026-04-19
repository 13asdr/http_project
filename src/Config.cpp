#include "config.h"

Config::Config(const std::string &_config)
{
    INIReader reader(_config);
    if (reader.ParseError() < 0)
    {
        throw std::runtime_error("Error parsing config file");
    }

    db.host = reader.Get("database", "host", "127.0.0.1");
    db.user = reader.Get("database", "user", "root");
    db.password = reader.Get("database", "password", "123456");
    db.database = reader.Get("database", "database", "accounting");
    db.port = reader.GetInteger("database", "port", 3306);

    server.host = reader.Get("server", "host", "0.0.0.0");
    server.port = reader.GetInteger("server", "port", 8080);
}
