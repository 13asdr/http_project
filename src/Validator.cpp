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

        const char *required_fields[] = {"note", "type", "category", "time"};
        for (const char *field : required_fields)
        {
            if (!_json.contains(field) || !_json[field].is_string() || _json[field].get<std::string>().empty())
            {
                return Validator::build_result(false, MessageCode::InvalidParam, std::string(field) + " is required");
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
