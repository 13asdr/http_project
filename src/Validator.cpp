#include "validator.h"

ValidationResult Validator::parse_positive_int(const std::string &_text, int &_value)
{
    try
    {
        _value = std::stoi(_text);
        if (_value <= 0)
        {
            return Validator::build_result(false, MessageCode::InvalidParam, "invalid positive integer");
        }
        return Validator::build_result(true, MessageCode::Success, "");
    }
    catch (const std::exception &_e)
    {
        (void)_e;
        return Validator::build_result(false, MessageCode::InvalidParam, "invalid positive integer");
    }
}

ValidationResult Validator::validate_limit(const Request &_req, Limit &_limit)
{
    if (!_req.has_param("page") || !_req.has_param("pageSize"))
    {
        return Validator::build_result(false, MessageCode::InvalidParam, "missing pagination parameters");
    }

    std::string page = _req.get_param_value("page");
    std::string page_size = _req.get_param_value("pageSize");
    if (page.empty() || page_size.empty())
    {
        return Validator::build_result(false, MessageCode::InvalidParam, "empty pagination parameters");
    }

    int parsed_page = 0;
    int parsed_page_size = 0;
    if (!Validator::parse_positive_int(page, parsed_page).is_valid || !Validator::parse_positive_int(page_size, parsed_page_size).is_valid)
    {
        return Validator::build_result(false, MessageCode::InvalidParam, "pagination parameters must be positive integers");
    }

    if (parsed_page_size > 100)
    {
        return Validator::build_result(false, MessageCode::InvalidParam, "pageSize must be between 1 and 100");
    }

    _limit = {parsed_page, parsed_page_size};
    return Validator::build_result(true, MessageCode::Success, "");
}

ValidationResult Validator::validate_record_json(const Json &_json)
{
    if (!_json.is_object())
    {
        return Validator::build_result(false, MessageCode::InvalidJson, "invalid record JSON");
    }

    try
    {
        if (!_json.contains("amount") || !_json["amount"].is_number())
        {
            return Validator::build_result(false, MessageCode::InvalidParam, "amount must be a number");
        }
        if (!_json.contains("note"))
        {
            return Validator::build_result(false, MessageCode::InvalidParam, "note is required");
        }
        if (!_json.contains("note") || !_json["note"].is_string() || _json["note"].get<std::string>().empty())
        {
            return Validator::build_result(false, MessageCode::InvalidParam, "note  must be a non-empty string");
        }

        const FieldValidator validators[] = {
            {"type", &Validator::validate_type, "type must be either 支出 or 收入"},
            {"category", &Validator::validate_category, "category must be one of 餐饮, 交通, 购物, 工资, 学习, 娱乐, 其他"},
            {"time", &Validator::validate_time, "time must be in format YYYY-MM-DDTHH:MM:SS"}};

        for (const auto &validator : validators)
        {
            if (!_json.contains(validator.field_name))
            {
                return Validator::build_result(false, MessageCode::InvalidParam, std::string(validator.field_name) + " is required");
            }

            if (!_json[validator.field_name].is_string() || !validator.validate(_json[validator.field_name].get<std::string>()))
            {
                return Validator::build_result(false, MessageCode::InvalidParam, validator.error_msg);
            }
        }

        return Validator::build_result(true, MessageCode::Success, "");
    }
    catch (const std::exception &_e)
    {
        (void)_e;
        return Validator::build_result(false, MessageCode::InvalidJson, "invalid record JSON");
    }
}

ValidationResult Validator::validate_user_json(const Json &_json)
{
    if (!_json.is_object())
    {
        return Validator::build_result(false, MessageCode::InvalidJson, "invalid user JSON");
    }

    try
    {
        if (!_json.contains("username") || !_json["username"].is_string())
        {
            return Validator::build_result(false, MessageCode::InvalidParam, "username is required");
        }
        if (!_json.contains("password") || !_json["password"].is_string())
        {
            return Validator::build_result(false, MessageCode::InvalidParam, "password is required");
        }

        std::string username = _json["username"];
        std::string password = _json["password"];

        if (username.length() < 3 || username.length() > 32)
        {
            return Validator::build_result(false, MessageCode::InvalidParam, "username length must be between 3 and 32");
        }
        if (password.length() < 6 || password.length() > 64)
        {
            return Validator::build_result(false, MessageCode::InvalidParam, "password length must be between 6 and 64");
        }

        return Validator::build_result(true, MessageCode::Success, "");
    }
    catch (const std::exception &_e)
    {
        (void)_e;
        return Validator::build_result(false, MessageCode::InvalidJson, "invalid user JSON");
    }
}

ValidationResult Validator::validate_token(const std::string &_token)
{
    if (_token.empty())
    {
        return Validator::build_result(false, MessageCode::Unauthorized, "Authorization header is required");
    }

    constexpr const char *bearer_prefix = "Bearer ";
    constexpr std::size_t bearer_prefix_length = 7;

    if (_token.rfind(bearer_prefix, 0) != 0)
    {
        return Validator::build_result(false, MessageCode::Unauthorized, "Authorization header must use Bearer token");
    }

    if (_token.size() <= bearer_prefix_length)
    {
        return Validator::build_result(false, MessageCode::Unauthorized, "Bearer token is required");
    }

    return Validator::build_result(true, MessageCode::Success, "");
}

bool Validator::validate_time(const std::string &time)
{
    std::regex time_regex(R"(^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}$)");
    return std::regex_match(time, time_regex);
}
bool Validator::validate_type(const std::string &time)
{
    return time == "支出" || time == "收入";
}
bool Validator::validate_category(const std::string &time)
{
    return time == "餐饮" || time == "交通" || time == "购物" || time == "工资" || time == "学习" || time == "娱乐" || time == "其他";
}
