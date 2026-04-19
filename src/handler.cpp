#include "handler.h"
#include "logger.h"

void Handler::add_record(RecordDao &_dao, const Request &_req, Response &_res)
{
    try
    {
        Logger::info("Handler::add_record , RecordAdd request received");

        if (!Json::accept(_req.body))
        {
            Logger::error("Handler::add_record , RecordAdd request body is not valid JSON");
            Handler::send_error(_res, HttpStatus::BadRequest, BusinessStatus::InvalidJSON, "invalid JSON");
            return;
        }

        auto j = Json::parse(_req.body);
        ValidationResult vr = Validator::validateRecordJson(j);
        if (!vr.is_valid)
        {
            Logger::error("Handler::add_record , RecordAdd request has invalid JSON parameters: " + vr.message);
            Handler::send_error(_res, HttpStatus::BadRequest, vr.code, vr.message);
            return;
        }

        auto userId = auth_check(_req, _res);
        if (userId == -1)
        {
            return;
        }

        Record r;
        json_to_record(j, r);
        r.user_id = userId;

        if (_dao.add(r))
        {
            Logger::info("Handler::Add , RecordAdd request processed successfully");
            Handler::send_success(_res, Json::object(), "record added successfully");
        }
        else
        {
            Logger::error("Handler::Add , RecordAdd request failed");
            Handler::send_error(_res, HttpStatus::InternalError, BusinessStatus::InternalError, "Failed to add record");
        }
    }
    catch (const std::exception &e)
    {
        Handler::handle_InternalError(_req, _res, e);
    }
}

void Handler::list_records(RecordDao &_dao, const Request &_req, Response &_res)
{
    try
    {
        Logger::info("Handler::list_records , Record list request received");

        auto userId = auth_check(_req, _res);
        if (userId == -1)
        {
            return;
        }

        Limit l;
        ValidationResult vr = Validator::validateLimit(_req, l);
        if (!vr.is_valid)
        {
            Logger::error("Handler::list_records , Record list request has invalid parameters: " + vr.message);
            Handler::send_error(_res, HttpStatus::BadRequest, vr.code, vr.message);
            return;
        }

        int count = _dao.count_records(userId);
        auto records = _dao.list_order_by_timeAndId(userId, l);
        Json records_json = Json::array();
        for (auto &r : records)
        {
            Json item;
            record_to_json(r, item);
            records_json.push_back(item);
        }

        Json res_json;
        res_json["total"] = count;
        res_json["page"] = l.page;
        res_json["pageSize"] = l.pageSize;
        res_json["records"] = records_json;

        Logger::info("Handler::list_records , Record list request processed successfully");
        Handler::send_success(_res, res_json, "record list retrieved successfully");
    }
    catch (const std::exception &e)
    {
        Handler::handle_InternalError(_req, _res, e);
    }
};

void Handler::stat_by_category(RecordDao &_dao, const Request &_req, Response &_res)
{
    try
    {
        Logger::info("Handler::stat_by_category , Record statByCategory request received");
        auto userId = auth_check(_req, _res);
        if (userId == -1)
        {
            return;
        }

        auto records = _dao.statByCategory(userId);

        Json result = Json::array();
        for (const auto &[key, value] : records)
        {
            Json item;
            item["category"] = key;
            item["total"] = value;
            result.push_back(item);
        }

        Logger::info("Handler::stat_by_category , Record statByCategory request processed successfully");
        Handler::send_success(_res, result, "record statByCategory retrieved successfully");
    }
    catch (const std::exception &e)
    {
        Handler::handle_InternalError(_req, _res, e);
    }
}

void Handler::filter_records(RecordDao &_dao, const Request &_req, Response &_res)
{
    try
    {
        Logger::info("Handler::filter_records , Record filter request received");
        auto userId = auth_check(_req, _res);
        if (userId == -1)
        {
            return;
        }

        Limit l;
        ValidationResult vr = Validator::validateLimit(_req, l);
        if (!vr.is_valid)
        {
            Logger::error("Handler::filter_records , Record filter request has invalid parameters: " + vr.message);
            Handler::send_error(_res, HttpStatus::BadRequest, vr.code, vr.message);
            return;
        }

        std::string keyword = _req.get_param_value("keyword");
        std::string month = _req.get_param_value("month");
        int count = _dao.count_records(userId, month, keyword);
        Json records_json = Json::array();

        for (auto &r : _dao.filter(keyword, month, userId, l))
        {
            Json item;
            record_to_json(r, item);
            records_json.push_back(item);
        }

        Json res_json;
        res_json["total"] = count;
        res_json["page"] = l.page;
        res_json["pageSize"] = l.pageSize;
        res_json["records"] = records_json;

        Logger::info("Handler::filter_records , Record filter request processed successfully");
        Handler::send_success(_res, res_json, "record filter retrieved successfully");
    }
    catch (const std::exception &e)
    {
        Handler::handle_InternalError(_req, _res, e);
    }
}

void Handler::update_record(RecordDao &_dao, const Request &_req, Response &_res)
{
    try
    {
        Logger::info("Handler::update_record , Record update request received");
        auto userId = auth_check(_req, _res);
        if (userId == -1)
        {
            return;
        }

        auto idIt = _req.params.find("id");
        if (idIt == _req.params.end())
        {
            Logger::error("Handler::update_record , Record update request missing id parameter");
            Handler::send_error(_res, HttpStatus::BadRequest, BusinessStatus::InvalidPARAM, "missing id parameter");
            return;
        }

        int id = 0;
        ValidationResult vr = Validator::parsePositiveInt(idIt->second, id);
        if (!vr.is_valid)
        {
            Logger::error("Handler::update_record , Record update request has invalid id parameter: " + idIt->second);
            Handler::send_error(_res, HttpStatus::BadRequest, vr.code, vr.message);
            return;
        }

        auto j = Json::parse(_req.body);
        vr = Validator::validateRecordJson(j);
        if (!vr.is_valid)
        {
            Logger::error("Handler::update_record , Record update request has invalid JSON parameters: " + vr.message);
            Handler::send_error(_res, HttpStatus::BadRequest, vr.code, vr.message);
            return;
        }

        Record r;
        json_to_record(j, r);
        r.user_id = userId;

        if (_dao.update(id, r, userId))
        {
            Logger::info("Handler::update_record , Record update request processed successfully");
            Handler::send_success(_res, Json::object(), "record updated successfully");
        }
        else
        {
            Logger::info("Handler::update_record , Record update request processed with failure");
            Handler::send_error(_res, HttpStatus::BadRequest, BusinessStatus::InvalidPARAM, "failed to update record");
        }
    }
    catch (const std::exception &e)
    {
        Handler::handle_InternalError(_req, _res, e);
    }
}

void Handler::remove_record(RecordDao &_dao, const Request &_req, Response &_res)
{
    try
    {
        Logger::info("Handler::remove_record , Record remove request received");
        auto userId = auth_check(_req, _res);
        if (userId == -1)
        {
            return;
        }

        auto idIt = _req.params.find("id");
        if (idIt == _req.params.end())
        {
            Logger::error("Handler::remove_record , Record remove request missing id parameter");
            Handler::send_error(_res, HttpStatus::BadRequest, BusinessStatus::InvalidPARAM, "missing id parameter");
            return;
        }

        int id = 0;
        ValidationResult vr = Validator::parsePositiveInt(idIt->second, id);
        if (!vr.is_valid)
        {
            Logger::error("Handler::remove_record , Record remove request has invalid id parameter");
            Handler::send_error(_res, HttpStatus::BadRequest, vr.code, vr.message);
            return;
        }

        Logger::info("Handler::remove_record , Record remove id param: " + std::to_string(id));
        if (_dao.remove(id, userId))
        {
            Logger::info("Handler::remove_record , Record remove request processed successfully");
            Handler::send_success(_res, Json::object(), "record deleted successfully");
        }
        else
        {
            Logger::info("Handler::remove_record , Record remove request processed with failure");
            Handler::send_error(_res, HttpStatus::BadRequest, BusinessStatus::InvalidPARAM, "failed to delete record");
        }
    }
    catch (const std::exception &e)
    {
        Handler::handle_InternalError(_req, _res, e);
    }
}

void Handler::export_records(RecordDao &_dao, const Request &_req, Response &_res)
{
    try
    {
        Logger::info("Handler::export_records , Record export request received");
        auto userId = auth_check(_req, _res);
        if (userId == -1)
        {
            return;
        }

        Logger::info("Handler::export_records , Record export user_id: " + std::to_string(userId));

        auto records = _dao.list_order_by_id(userId);

        Logger::info("Handler::export_records , Record export records: " + std::to_string(records.size()));

        std::ostringstream oss;
        oss << "\xEF\xBB\xBF";
        oss << "id,金额,备注,类型,日期,分类\n";

        for (const auto &r : records)
        {
            oss << r.id << "," << r.amount << ",\"" << r.note << "\"," << r.type << "," << r.time << "," << r.category << "\n";
        }

        Logger::info("Handler::export_records , Record export records size: " + std::to_string(records.size()));

        _res.set_content(oss.str(), "text/csv");
        _res.set_header("Content-Disposition", "attachment; filename=\"records.csv\"");
    }
    catch (const std::exception &e)
    {
        Handler::handle_InternalError(_req, _res, e);
    }
}

void Handler::add_user(UserDao &_dao, const Request &_req, Response &_res)
{
    try
    {
        Logger::info("Handler::add_user , User Add request received");
        if (!Json::accept(_req.body))
        {
            Logger::error("Handler::add_user , User Add request body is not valid JSON");
            Handler::send_error(_res, HttpStatus::BadRequest, BusinessStatus::InvalidJSON, "invalid JSON");
            return;
        }

        auto j = Json::parse(_req.body);
        ValidationResult vr = Validator::validateUserJson(j);
        if (!vr.is_valid)
        {
            Logger::error("Handler::add_user , User Add request has invalid JSON parameters: " + vr.message);
            Handler::send_error(_res, HttpStatus::BadRequest, vr.code, vr.message);
            return;
        }

        User user;
        json_to_user(j, user);
        user.password = Crypto::sha256(user.password);

        if (!_dao.add(user))
        {
            Logger::error("Handler::add_user , User Add request processed failed");
            Handler::send_error(_res, HttpStatus::InternalError, BusinessStatus::InternalError, "failed to add user");
            return;
        }
        Logger::info("Handler::add_user , User Add request processed successfully");
        Handler::send_success(_res, Json::object(), "user added successfully");
    }
    catch (const std::exception &e)
    {
        Handler::handle_InternalError(_req, _res, e);
    }
}

void Handler::update_user(UserDao &_dao, const Request &_req, Response &_res)
{
    try
    {
        Logger::info("Handler::update_user , User Update request received");
        if (!Json::accept(_req.body))
        {
            Logger::error("Handler::update_user , User Update request body is not valid JSON");
            Handler::send_error(_res, HttpStatus::BadRequest, BusinessStatus::InvalidJSON, "invalid JSON");
            return;
        }
        auto j = Json::parse(_req.body);
        ValidationResult vr = Validator::validateUserJson(j);
        if (!vr.is_valid)
        {
            Logger::error("Handler::update_user , User Update request has invalid JSON parameters: " + vr.message);
            Handler::send_error(_res, HttpStatus::BadRequest, vr.code, vr.message);
            return;
        }

        User user;
        json_to_user(j, user);
        user.password = Crypto::sha256(user.password);

        if (!_dao.update(user))
        {
            Logger::error("Handler::update_user , User Update request processed failed");
            Handler::send_error(_res, HttpStatus::InternalError, BusinessStatus::InternalError, "failed to update user");
            return;
        }
        Logger::info("Handler::update_user , User Update request processed successfully");
        Handler::send_success(_res, Json::object(), "user updated successfully");
    }
    catch (const std::exception &e)
    {
        Handler::handle_InternalError(_req, _res, e);
    }
}

void Handler::remove_user(UserDao &_dao, const Request &_req, Response &_res)
{
    try
    {
        Logger::info("Handler::remove_user , User Remove request received");
        if (_req.params.find("username") == _req.params.end())
        {
            Logger::error("Handler::remove_user , User Remove request missing username parameter");
            Handler::send_error(_res, HttpStatus::BadRequest, BusinessStatus::InvalidPARAM, "missing username parameter");
            return;
        }

        std::string username = _req.get_param_value("username");
        if (username.empty())
        {
            Logger::error("Handler::remove_user , User Remove request has empty username parameter");
            Handler::send_error(_res, HttpStatus::BadRequest, BusinessStatus::InvalidPARAM, "invalid username parameter");
            return;
        }

        if (!_dao.remove(username))
        {
            Logger::error("Handler::remove_user , User Remove request processed failed");
            Handler::send_error(_res, HttpStatus::InternalError, BusinessStatus::InternalError, "failed to delete user");
            return;
        }
        Logger::info("Handler::remove_user , User Remove request processed successfully");
        Handler::send_success(_res, Json::object(), "user deleted successfully");
    }
    catch (const std::exception &e)
    {
        Handler::handle_InternalError(_req, _res, e);
    }
}

void Handler::login_user(UserDao &_dao, const Request &_req, Response &_res)
{
    try
    {
        Logger::info("Handler::login_user , User Login request received");

        if (!Json::accept(_req.body))
        {
            Logger::error("Handler::login_user , User Login request body is not valid JSON");
            Handler::send_error(_res, HttpStatus::BadRequest, BusinessStatus::InvalidJSON, "invalid JSON");
            return;
        }

        auto j = Json::parse(_req.body);
        ValidationResult vr = Validator::validateUserJson(j);
        if (!vr.is_valid)
        {
            Logger::error("Handler::login_user , User Login request has invalid JSON parameters: " + vr.message);
            Handler::send_error(_res, HttpStatus::BadRequest, vr.code, vr.message);
            return;
        }

        User user;
        json_to_user(j, user);
        auto userOptional = _dao.query(user.username);
        auto hashedPassword = Crypto::sha256(user.password);

        if (!userOptional.has_value() || userOptional.value().password != hashedPassword)
        {
            Handler::send_error(_res, HttpStatus::Unauthorized, BusinessStatus::Unauthorized, "invalid username or password");
            Logger::error("Handler::login_user , User Login failed");
            return;
        }
        Json data;
        data["token"] = TokenManager::generate_token(userOptional.value().id);
        Handler::send_success(_res, data, "login success");
        Logger::info("Handler::login_user , User Login success");
    }
    catch (const std::exception &e)
    {
        Handler::handle_InternalError(_req, _res, e);
    }
}

void Handler::register_user(UserDao &_dao, const Request &_req, Response &_res)
{
    try
    {
        Logger::info("Handler::register_user , User Register request received");
        if (!Json::accept(_req.body))
        {
            Logger::error("Handler::register_user , User Register request body is not valid JSON");
            Handler::send_error(_res, HttpStatus::BadRequest, BusinessStatus::InvalidJSON, "invalid JSON");
            return;
        }

        auto j = Json::parse(_req.body);
        ValidationResult vr = Validator::validateUserJson(j);
        if (!vr.is_valid)
        {
            Logger::error("Handler::register_user , User Register request has invalid JSON parameters: " + vr.message);
            Handler::send_error(_res, HttpStatus::BadRequest, vr.code, vr.message);
            return;
        }

        User user;
        json_to_user(j, user);
        auto userOptional = _dao.query(user.username);

        if (userOptional.has_value())
        {
            Logger::error("Handler::register_user , User Register failed");
            Handler::send_error(_res, HttpStatus::BadRequest, BusinessStatus::InvalidPARAM, "username already exists");
            return;
        }
        user.password = Crypto::sha256(user.password);
        _dao.add(user);
        Logger::info("Handler::register_user , User Register success");
        Handler::send_success(_res, Json::object(), "user registered successfully");
    }
    catch (const std::exception &e)
    {
        Handler::handle_InternalError(_req, _res, e);
    }
}

void Handler::logout_user(const Request &_req, Response &_res)
{
    try
    {
        Logger::info("Handler::logout_user , User Logout request received");
        auto token = _req.get_header_value("token");
        if (TokenManager::validate_token(token) != -1)
        {
            TokenManager::remove_token(token);
            Logger::info("Handler::logout_user , User Logout success");
            Handler::send_success(_res, Json::object(), "logout success");
        }
        else
        {
            Logger::info("Handler::logout_user , User Logout failed");
            Handler::send_error(_res, HttpStatus::Unauthorized, BusinessStatus::Unauthorized, "invalid token");
        }
    }
    catch (const std::exception &e)
    {
        Handler::handle_InternalError(_req, _res, e);
    }
}

int Handler::auth_check(const Request &_req, Response &_res)
{
    auto token = _req.get_header_value("token");
    auto userId = TokenManager::validate_token(token);
    if (userId != -1)
    {
        Logger::info("Handler::auth_check , User auth_check success, userId: " + std::to_string(userId));
        return userId;
    }

    Logger::info("Handler::auth_check , User auth_check failed, token is invalid");
    Handler::send_error(_res, HttpStatus::Unauthorized, BusinessStatus::Unauthorized, "invalid token");
    return -1;
}

void Handler::handle_InternalError(const Request &_req, Response &_res, const std::exception &e)
{
    Logger::error("Location : " + _req.path + " , Exception: " + e.what());
    Handler::send_error(_res, HttpStatus::InternalError, BusinessStatus::InternalError, "internal server error");
}

void Handler::json_to_record(Json &_j, Record &_r)
{
    _r.amount = _j["amount"];
    _r.note = _j["note"];
    _r.type = _j["type"];
    _r.time = _j["time"];
    _r.category = _j["category"];
}

void Handler::record_to_json(const Record &_r, Json &_j)
{
    _j["id"] = _r.id;
    _j["amount"] = _r.amount;
    _j["note"] = _r.note;
    _j["type"] = _r.type;
    _j["time"] = _r.time;
    _j["category"] = _r.category;
}

void Handler::json_to_user(Json &_j, User &_u)
{
    _u.username = _j["username"];
    _u.password = _j["password"];
}

void Handler::send_success(Response &_res, const Json &_data, const std::string &_message)
{
    Handler::Json result;
    result["code"] = 0;
    result["message"] = _message;
    result["data"] = _data;
    _res.set_content(result.dump(), "application/json");
    _res.status = static_cast<int>(HttpStatus::Success);    
}

void Handler::send_error(Response &_res, HttpStatus status, BusinessStatus code, const std::string &_message)
{
    Handler::Json result;
    result["code"] = static_cast<int>(code);
    result["message"] = _message;
    result["data"] = nullptr;
    _res.set_content(result.dump(), "application/json");
    _res.status = static_cast<int>(status);
}
