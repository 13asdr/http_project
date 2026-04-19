#pragma once
#include <string>
#include <iostream>
#include "inih/ini.h"
#include "inih/INIReader.h"

struct DBconfig
{
    std::string host;
    std::string user;
    std::string password;
    std::string database;
    size_t port;
    const char *__unix_socket = nullptr;
    size_t __client_flag = 0;
};

struct ServerConfig
{
    std::string host;
    size_t port;
};

class Config
{
public:
    DBconfig db;
    ServerConfig server;

    Config(const std::string &_config);
};
