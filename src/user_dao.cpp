#include "user_dao.h"

UserDao::UserDao(DbConnect &_db) : db(_db) {}

UserDao::~UserDao() {}

bool UserDao::add(const User &_user)
{
    std::ostringstream sql;
    sql << "INSERT INTO users (username, password) VALUES ('" << _user.username << "', '" << _user.password << "')";
    return db.execute(sql.str());
}

std::optional<User> UserDao::query(const std::string &_username)
{
    MYSQL_RES *res = db.query("SELECT id, username, password FROM users WHERE username = '" + _username + "'");
    if (!res)
    {
        return std::nullopt;
    }

    MYSQL_ROW row = mysql_fetch_row(res);
    if (row == nullptr)
    {
        mysql_free_result(res);
        return std::nullopt;
    }

    User user;
    user.id = std::stoi(row[0]);
    user.username = row[1];
    user.password = row[2];
    mysql_free_result(res);
    return user;
}

bool UserDao::update(const User &_user)
{
    std::ostringstream sql;
    sql << "UPDATE users SET username = '" << _user.username << "', password = '" << _user.password << "' WHERE username = '" << _user.username << "'";
    return db.execute(sql.str());
}

bool UserDao::remove(const std::string &_username)
{
    std::ostringstream sql;
    sql << "DELETE FROM users WHERE username = '" << _username << "'";
    return db.execute(sql.str());
}
