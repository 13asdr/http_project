#include "user_dao.h"

UserDao::UserDao(DBconnect &db) : db(db) {}

UserDao::~UserDao() {}

bool UserDao::add(const User &user)
{
    std::ostringstream sql;
    sql << "INSERT INTO users (username, password) VALUES ('" << user.username << "', '" << user.password << "')";
    return db.execute(sql.str());
}

std::optional<User> UserDao::query(const std::string &username)
{
    MYSQL_RES *res = db.query("SELECT id, username, password FROM users WHERE username = '" + username + "'");
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

bool UserDao::update(const User &user)
{
    std::ostringstream sql;
    sql << "UPDATE users SET username = '" << user.username << "', password = '" << user.password << "' WHERE username = '" << user.username << "'";
    return db.execute(sql.str());
}

bool UserDao::remove(const std::string &username)
{
    std::ostringstream sql;
    sql << "DELETE FROM users WHERE username = '" << username << "'";
    return db.execute(sql.str());
}
