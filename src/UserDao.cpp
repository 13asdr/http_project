#include "UserDao.h"

UserDao::UserDao(DBconnect &db) : db(db) {}

UserDao::~UserDao() {}
// 添加
bool UserDao::add(const User &user)
{
    std::ostringstream sql;
    sql << "INSERT INTO users (username, password) VALUES ('" << user.username << "', '" << user.password << "')";
    return db.execute(sql.str());
}

std::optional<User> UserDao::query(const std::string &username)
{
    User user;
    MYSQL_RES *res = db.query("SELECT id, username, password FROM users WHERE username = '" + username + "'");
    if (!res)
    {
        return std::nullopt; // 查询失败
    }
    MYSQL_ROW row = mysql_fetch_row(res);
    if (row == nullptr)
    {
        return std::nullopt; // 查询失败
    }
    mysql_free_result(res);
    user.id = std::stoi(row[0]);
    user.username = row[1];
    user.password = row[2];
    return user;
}

// 更新
bool UserDao::update(const User &user)
{
    std::ostringstream sql;
    sql << "UPDATE users SET username = '" << user.username << "', password = '" << user.password << "' WHERE username = '" << user.username << "'";
    return db.execute(sql.str());
}
// 删除
bool UserDao::remove(const std::string &username)
{
    std::ostringstream sql;
    sql << "DELETE FROM users WHERE username = '" << username << "'";
    return db.execute(sql.str());
}