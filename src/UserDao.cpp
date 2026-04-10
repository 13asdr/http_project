#include "UserDao.h"

UserDao::UserDao(DBconnect &db) : db(db) {}

UserDao::~UserDao() {}
// 添加
bool UserDao::add(const User &user)
{
    std::ostringstream sql;
    sql << "INSERT INTO users (name, password) VALUES ('" << user.name << "', '" << user.password << "')";
    return db.execute(sql.str());
}

bool UserDao::query(const std::string &username)
{
    MYSQL_RES *res = db.query("SELECT id FROM users WHERE name = '" + username + "'");
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
    sql << "UPDATE users SET name = '" << user.name << "', password = '" << user.password << "' WHERE name = '" << user.name << "'";
    return db.execute(sql.str());
}
// 删除
bool UserDao::remove(const std::string &username)
{
    std::ostringstream sql;
    sql << "DELETE FROM users WHERE name = '" << username << "'";
    return db.execute(sql.str());
}