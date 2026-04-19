

#pragma once
#include <string>
#include <vector>
#include <mysql.h>
#include <sstream>
#include <map>
#include <optional>

#include "db_connect.h"
#include "crypto.h"

struct User
{
    int id;
    std::string username;
    std::string password;
};

class UserDao
{
public:
    using password_Crypto = std::string;

    UserDao(DBconnect &_db);
    ~UserDao();

    // 添加
    bool add(const User &_user);

    // 查询
    std::optional<User> query(const std::string &_username); // 返回用户ID，-1表示查询失败

    // 更新
    bool update(const User &_user);

    // 删除
    bool remove(const std::string &_username);

private:
    DBconnect &db;
};
