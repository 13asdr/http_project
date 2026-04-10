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

bool UserDao::query(const std::string &username)
{
    MYSQL_RES *res = db.query("SELECT id FROM users WHERE username = '" + username + "'");
    if (!res)
    {
        return false;
    }
    MYSQL_ROW row = mysql_fetch_row(res);
    mysql_free_result(res);
    return row != nullptr;
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