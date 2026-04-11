

#pragma once
#include <string>
#include <vector>
#include <mysql.h>
#include <sstream>
#include <map>
#include <optional>

#include "DBconnect.h"

struct User
{
    int id;
    std::string username;
    std::string password;
};

class UserDao
{
public:
    UserDao(DBconnect &db);
    ~UserDao();

    // 添加
    bool add(const User &user);

    // 查询
    std::optional<User> query(const std::string &username); // 返回用户ID，-1表示查询失败

    // 更新
    bool update(const User &user);

    // 删除
    bool remove(const std::string &username);

private:
    DBconnect &db;
};
