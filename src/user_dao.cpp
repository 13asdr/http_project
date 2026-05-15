#include "user_dao.h"

UserDao::UserDao(DbConnect &_db) : db(_db) {}

UserDao::~UserDao() {}

bool UserDao::add(const User &_user)
{
    std::string escaped_username = escape_string(_user.username);
    std::string escaped_password = escape_string(_user.password);

    std::ostringstream sql;
    sql << "INSERT INTO users (username, password) VALUES ('" << escaped_username << "', '" << escaped_password << "')";
    return db.execute(sql.str());
}

std::optional<User> UserDao::query(const std::string &_username)
{
    std::string escaped_username = escape_string(_username);

    MYSQL_RES *res = db.query("SELECT id, username, password FROM users WHERE username = '" + escaped_username + "'"); // 有隐患
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
    User escaped_user = escape_user(_user);

    std::ostringstream sql;
    sql << "UPDATE users SET username = '" << escaped_user.username << "', password = '" << escaped_user.password << "' WHERE username = '" << escaped_user.username << "'";
    return db.execute(sql.str());
}

bool UserDao::remove(const std::string &_username)
{
    std::string escaped_username = escape_string(_username);

    std::ostringstream sql;
    sql << "DELETE FROM users WHERE username = '" << escaped_username << "'";
    return db.execute(sql.str());
}

User UserDao::escape_user(const User &_user)
{
    User escaped_user;
    escaped_user.id = _user.id;
    escaped_user.username = escape_string(_user.username);
    escaped_user.password = escape_string(_user.password);
    return escaped_user;
}

std::string UserDao::escape_string(const std::string &_input)
{
    return db.escape_string(_input);
}
