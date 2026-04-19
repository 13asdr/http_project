#pragma once

#include <mysql.h>

#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "crypto.h"
#include "db_connect.h"

struct User
{
    int id;
    std::string username;
    std::string password;
};

class UserDao
{
public:
    using PasswordCrypto = std::string;

    explicit UserDao(DbConnect &_db);
    ~UserDao();

    bool add(const User &_user);
    std::optional<User> query(const std::string &_username);
    bool update(const User &_user);
    bool remove(const std::string &_username);

private:
    DbConnect &db;
};
