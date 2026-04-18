#include "Validator.h"

ValidationResult Validator::parsePositiveInt(const std::string &text, int &value)
{
    try
    {
        value = std::stoi(text);
        if (value <= 0)
        {
            return Validator::buildResult(false, message_code::InvalidPARAM, "invalid positive integer");
        }
        return Validator::buildResult(true, message_code::Success, "");
    }
    catch (const std::exception &e)
    {
        (void)e;
        return Validator::buildResult(false, message_code::InvalidPARAM, "invalid positive integer");
    }
}

ValidationResult Validator::validateLimit(const Request &req, limit &l)
{
    if (!req.has_param("page") || !req.has_param("pageSize"))
    {
        return Validator::buildResult(false, message_code::InvalidPARAM, "missing pagination parameters");
    }

    std::string pageStr = req.get_param_value("page");
    std::string pageSizeStr = req.get_param_value("pageSize");
    if (pageStr.empty() || pageSizeStr.empty())
    {
        return Validator::buildResult(false, message_code::InvalidPARAM, "empty pagination parameters");
    }

    int page = 0;
    int pageSize = 0;
    if (!Validator::parsePositiveInt(pageStr, page).is_valid || !Validator::parsePositiveInt(pageSizeStr, pageSize).is_valid)
    {
        return Validator::buildResult(false, message_code::InvalidPARAM, "pagination parameters must be positive integers");
    }

    if (pageSize > 100)
    {
        return Validator::buildResult(false, message_code::InvalidPARAM, "pageSize must be between 1 and 100");
    }

    l = {page, pageSize};
    return Validator::buildResult(true, message_code::Success, "");
}

ValidationResult Validator::validateRecordJson(const Json &j)
{
    if (!j.is_object())
    {
        return Validator::buildResult(false, message_code::InvalidJSON, "invalid record JSON");
    }

    try
    {
        if (!j.contains("amount") || !j["amount"].is_number())
        {
            return Validator::buildResult(false, message_code::InvalidPARAM, "amount must be a number");
        }

        const char *requiredFields[] = {"note", "type", "category", "time"};
        for (const char *field : requiredFields)
        {
            if (!j.contains(field) || !j[field].is_string() || j[field].get<std::string>().empty())
            {
                return Validator::buildResult(false, message_code::InvalidPARAM, std::string(field) + " is required");
            }
        }

        return Validator::buildResult(true, message_code::Success, "");
    }
    catch (const std::exception &e)
    {
        (void)e;
        return Validator::buildResult(false, message_code::InvalidJSON, "invalid record JSON");
    }
}

ValidationResult Validator::validateUserJson(const Json &j)
{
    if (!j.is_object())
    {
        return Validator::buildResult(false, message_code::InvalidJSON, "invalid user JSON");
    }

    try
    {
        if (!j.contains("username") || !j["username"].is_string())
        {
            return Validator::buildResult(false, message_code::InvalidPARAM, "username is required");
        }
        if (!j.contains("password") || !j["password"].is_string())
        {
            return Validator::buildResult(false, message_code::InvalidPARAM, "password is required");
        }

        std::string username = j["username"];
        std::string password = j["password"];

        if (username.length() < 3 || username.length() > 32)
        {
            return Validator::buildResult(false, message_code::InvalidPARAM, "username length must be between 3 and 32");
        }
        if (password.length() < 6 || password.length() > 64)
        {
            return Validator::buildResult(false, message_code::InvalidPARAM, "password length must be between 6 and 64");
        }

        return Validator::buildResult(true, message_code::Success, "");
    }
    catch (const std::exception &e)
    {
        (void)e;
        return Validator::buildResult(false, message_code::InvalidJSON, "invalid user JSON");
    }
}
