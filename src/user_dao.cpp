#include "user_dao.h"

UserDao::UserDao(DbConnect &_db) : db(_db)
{
}

UserDao::~UserDao() {}

bool UserDao::add(const User &_user)
{
    constexpr const char* sql = "INSERT INTO users (username, password) VALUES (?, ?)";
    PreparedStmt stmt(db.get_conn(), sql);

    const char *username = _user.username.c_str();
    const char *password = _user.password.c_str();

    MYSQL_BIND input[2] = {};
    input[0].buffer_type = MYSQL_TYPE_STRING;
    input[0].buffer = (void *)username;
    input[0].buffer_length = static_cast<unsigned long>(strlen(username));

    input[1].buffer_type = MYSQL_TYPE_STRING;
    input[1].buffer = (void *)password;
    input[1].buffer_length = static_cast<unsigned long>(strlen(password));

    if (!stmt.bind_param(input))
    {
        return false;
    }
    return stmt.execute();
}

std::optional<User> UserDao::query(const std::string &_username)
{
    PreparedStmt stmt(db.get_conn(), "SELECT id, username, password FROM users WHERE username = ?");
    const char *username = _username.c_str();

    MYSQL_BIND input[1] = {};
    input[0].buffer_type = MYSQL_TYPE_STRING;
    input[0].buffer = (void *)username;
    input[0].buffer_length = static_cast<unsigned long>(strlen(username));

    if (!stmt.bind_param(input))
    {
        return std::nullopt;
    }

    int id = 0;
    char username_buffer[256];
    char password_buffer[256];
    unsigned long username_len, password_len;
    MYSQL_BIND output[3] = {};
    output[0].buffer_type = MYSQL_TYPE_LONG;
    output[0].buffer = (void *)&id;
    output[0].buffer_length = sizeof(id);

    output[1].buffer_type = MYSQL_TYPE_STRING;
    output[1].buffer = username_buffer;
    output[1].buffer_length = sizeof(username_buffer);
    output[1].length = &username_len;

    output[2].buffer_type = MYSQL_TYPE_STRING;
    output[2].buffer = password_buffer;
    output[2].buffer_length = sizeof(password_buffer);
    output[2].length = &password_len;

    if (!stmt.bind_result(output))
    {
        return std::nullopt;
    }

    return stmt.fetch_one<User>([&](MYSQL_STMT *stmt) -> User
                                {
        User user;
        user.id = id;
        user.username = std::string(username_buffer, username_len);
        user.password = std::string(password_buffer, password_len);
        return user; });
}

bool UserDao::update(const User &_user)
{
    constexpr const char* sql = "UPDATE users SET password = ? WHERE username = ?";
    PreparedStmt stmt(db.get_conn(), sql);

    const char *username = _user.username.c_str();
    const char *password = _user.password.c_str();

    MYSQL_BIND input[2] = {};
    input[0].buffer_type = MYSQL_TYPE_STRING;
    input[0].buffer = (void *)username;
    input[0].buffer_length = static_cast<unsigned long>(strlen(username));

    input[1].buffer_type = MYSQL_TYPE_STRING;
    input[1].buffer = (void *)password;
    input[1].buffer_length = static_cast<unsigned long>(strlen(password));

    if (!stmt.bind_param(input))
    {
        return false;
    }
    return stmt.execute();
}

bool UserDao::remove(const std::string &_username)
{
    constexpr const char* sql = "DELETE FROM users WHERE username = ?";
    PreparedStmt stmt(db.get_conn(), sql);
    const char *username = _username.c_str();

    MYSQL_BIND input[1] = {};
    input[0].buffer_type = MYSQL_TYPE_STRING;
    input[0].buffer = (void *)username;
    input[0].buffer_length = static_cast<unsigned long>(strlen(username));

    if (!stmt.bind_param(input))
    {
        return false;
    }
    return stmt.execute();
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
