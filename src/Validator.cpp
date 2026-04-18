#include "Validator.h"

bool Validator::parsePositiveInt(const std::string &text, int &value)
{
    try
    {
        value = std::stoi(text);
        return value >= 1;
    }
    catch (const std::exception &e)
    {
        Logger::error("Validator::parsePositiveInt , Exception: " + std::string(e.what()) + " , input : " + text);
        return false;
    }
}
bool Validator::validateLimit(const Request &req, Response &res, limit &l) //
{

    if (!req.has_param("page") || !req.has_param("pageSize"))
    {
        Logger::error("Validator::validateLimit , pagination parameters are missing");
        Handler::sendError(res, http_status::bad_request, message_code::InvalidPARAM, "invalid pagination parameters");
        return true;
    }

    std::string pageStr = req.get_param_value("page");
    std::string pageSizeStr = req.get_param_value("pageSize");
    if (pageStr.empty() || pageSizeStr.empty())
    {
        Logger::error("Validator::validateLimit , pagination parameters are empty");
        Handler::sendError(res, http_status::bad_request, message_code::InvalidPARAM, "invalid pagination parameters");
        return false;
    }

    int _page = 0;
    int _pageSize = 0;
    if (Validator::parsePositiveInt(pageStr, _page) == false || Validator::parsePositiveInt(pageSizeStr, _pageSize) == false)
    {
        Logger::error("Validator::validateLimit , pagination parameters are not positive integers");
        Handler::sendError(res, http_status::bad_request, message_code::InvalidPARAM, "invalid pagination parameters");
        return false;
    }

    if (_page >= 1 && (_pageSize >= 1 && _pageSize <= 100))
    {
        l = {_page, _pageSize};
        Logger::info("Validator::validateLimit , pagination parameters are valid");
        return true;
    }
    else
    {
        Logger::error("Validator::validateLimit , invalid pagination parameters: page=" + std::to_string(_page) + ", pageSize=" + std::to_string(_pageSize));
        Handler::sendError(res, http_status::bad_request, message_code::InvalidPARAM, "invalid pagination parameters");
        return false;
    }
}

bool Validator::validateRecordJson(const Json &j, Response &res)
{
    try
    {
        if (j["amount"].is_number() && (!j["note"].is_null() && !j["type"].is_null() && !j["category"].is_null() && !j["time"].is_null() ) && (!j["note"].empty() && !j["type"].empty() && !j["category"].empty() && !j["time"].empty()))
        {
            Logger::info("Validator::validateRecordJson , record JSON parameters are valid");
            return true;
        }
        Logger::error("Validator::validateRecordJson , invalid record JSON parameters");
        Handler::sendError(res, http_status::bad_request, message_code::InvalidPARAM, "invalid record JSON parameters");
        return false;
    }
    catch (const std::exception &e)
    {
        Logger::error("Validator::validateRecordJson , Exception: " + std::string(e.what()));
        Handler::sendError(res, http_status::bad_request, message_code::InvalidJSON, "invalid JSON format for record");
        return false;
    }
}

bool Validator::validateUserJson(const Json &j, Response &res)
{
    try
    {
        std::string username = j["username"];
        std::string password = j["password"];

        size_t len_name = username.length();
        size_t len_password = password.length();

        if ((len_name >= 3 && len_name <= 32) && (len_password >= 6 && len_password <= 64))
        {

            Logger::info("Validator::validateUserJson , user JSON parameters are valid");
            return true;
        }
        Logger::error("Validator::validateUserJson , invalid username or password length");
        Handler::sendError(res, http_status::bad_request, message_code::InvalidPARAM, "invalid username or password length");
        return false;
    }
    catch (const std::exception &e)
    {
        Logger::error("Validator::validateUserJson , Exception: " + std::string(e.what()));
        Handler::sendError(res, http_status::bad_request, message_code::InvalidJSON, "invalid JSON format for user");
        return false;
    }
}