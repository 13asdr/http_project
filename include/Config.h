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
    int port;
    const char *unix_socket = nullptr;
    unsigned long client_flag = 0;
};

struct ServerConfig
{
    std::string host;
    int port;
};

class Config
{
public:
    DbConfig db;
    ServerConfig server;

    explicit Config(const std::string &_config);
};

class JwtConfig
{
public:
    explicit JwtConfig(const std::string &_config_file);

    void load_config(const std::string &_config_file);

    const std::string &get_secret_key() const { return secret_key; }

private:
    std::string secret_key;
};
