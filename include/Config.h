#pragma once

#include <string>
#include <stdexcept>

#include "inih/INIReader.h"
#include "inih/ini.h"

struct DbConfig
{
    std::string host;
    std::string user;
    std::string password;
    std::string database;
    size_t port;
    const char *unix_socket = nullptr;
    size_t client_flag = 0;
};

struct ServerConfig
{
    std::string host;
    size_t port;
};

class Config
{
public:
    DbConfig db;
    ServerConfig server;

    explicit Config(const std::string &_config);
};
