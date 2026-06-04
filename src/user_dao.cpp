#include "user_dao.h"

UserDao::UserDao(DbConnect &_db) : db(_db)
{
}

UserDao::~UserDao() {}

bool UserDao::add(const User &_user)
{
    constexpr const char *sql = "INSERT INTO users (username, password) VALUES (?, ?)";
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

    MYSQL_BIND input = createBindHelper::create_input_bind(username, MYSQL_TYPE_STRING);

    if (!stmt.bind_param(&input))
    {
        return std::nullopt;
    }

    if (!stmt.execute())
    {
        return std::nullopt;
    }

    int id = 0;
    char username_buffer[256];
    char password_buffer[256];
    unsigned long username_len, password_len;

    std::vector<MYSQL_BIND> output;
    output.reserve(3);
    output.push_back(createBindHelper::create_output_bind(id, MYSQL_TYPE_LONG));
    output.push_back(createBindHelper::create_output_bind(username_buffer, username_len, MYSQL_TYPE_STRING));
    output.push_back(createBindHelper::create_output_bind(password_buffer, password_len, MYSQL_TYPE_STRING));

    if (!stmt.bind_result(output.data()))
    {
        return std::nullopt;
    }

    if (!stmt.fetch_row())
    {
        return std::nullopt;
    }

    User user;
    user.id = id;
    user.username = std::string(username_buffer, username_len);
    user.password = std::string(password_buffer, password_len);
    return user;
}

bool UserDao::update(const User &_user)
{
    constexpr const char *sql = "UPDATE users SET password = ? WHERE username = ?";
    PreparedStmt stmt(db.get_conn(), sql);

    const char *password = _user.password.c_str();
    const char *username = _user.username.c_str();

    std::vector<MYSQL_BIND> input;
    input.reserve(2);

    input.push_back(createBindHelper::create_input_bind(password, MYSQL_TYPE_STRING));
    input.push_back(createBindHelper::create_input_bind(username, MYSQL_TYPE_STRING));

    if (!stmt.bind_param(input.data()))
    {
        return false;
    }
    return stmt.execute();
}

bool UserDao::remove(const std::string &_username)
{
    constexpr const char *sql = "DELETE FROM users WHERE username = ?";
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

