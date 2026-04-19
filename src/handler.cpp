#include "handler.h"
#include "logger.h"

void Handler::Add(RecordDao &_dao, const Request &_req, Response &_res)
{
    try
    {
        Logger::info("Handler::Add , RecordAdd request received");

        if (!Json::accept(_req.body))
        {
            Logger::error("Handler::Add , RecordAdd request body is not valid JSON");
            Handler::sendError(_res, HttpStatus::bad_request, BusinessStatus::InvalidJSON, "invalid JSON");
            return;
        }

        auto j = Json::parse(_req.body);
        ValidationResult vr = Validator::validateRecordJson(j);
        if (!vr.is_valid)
        {
            Logger::error("Handler::Add , RecordAdd request has invalid JSON parameters: " + vr.message);
            Handler::sendError(_res, HttpStatus::bad_request, vr.code, vr.message);
            return;
        }

        auto userId = authCheck(_req, _res);
        if (userId == -1)
        {
            return;
        }

        Record r;
        JsonToRecord(j, r);
        r.user_id = userId;

        if (_dao.add(r))
        {
            Logger::info("Handler::Add , RecordAdd request processed successfully");
            Handler::sendSuccess(_res, Json::object(), "record added successfully");
        }
        else
        {
            Logger::error("Handler::Add , RecordAdd request failed");
            Handler::sendError(_res, HttpStatus::internal_error, BusinessStatus::InternalError, "Failed to add record");
        }
    }
    catch (const std::exception &e)
    {
        Handler::handleInternalError(_req, _res, e);
    }
}

void Handler::List(RecordDao &_dao, const Request &_req, Response &_res)
{
    try
    {
        Logger::info("Handler::List , Record list request received");

        auto userId = authCheck(_req, _res);
        if (userId == -1)
        {
            return;
        }

        Limit l;
        ValidationResult vr = Validator::validateLimit(_req, l);
        if (!vr.is_valid)
        {
            Logger::error("Handler::List , Record list request has invalid parameters: " + vr.message);
            Handler::sendError(_res, HttpStatus::bad_request, vr.code, vr.message);
            return;
        }

        int count = _dao.count_records(userId);
        auto records = _dao.list_order_by_timeAndId(userId, l);
        Json records_json = Json::array();
        for (auto &r : records)
        {
            Json item;
            RecordToJson(r, item);
            records_json.push_back(item);
        }

        Json res_json;
        res_json["total"] = count;
        res_json["page"] = l.page;
        res_json["pageSize"] = l.pageSize;
        res_json["records"] = records_json;

        Logger::info("Handler::List , Record list request processed successfully");
        Handler::sendSuccess(_res, res_json, "record list retrieved successfully");
    }
    catch (const std::exception &e)
    {
        Handler::handleInternalError(_req, _res, e);
    }
};

void Handler::StatByCategory(RecordDao &_dao, const Request &_req, Response &_res)
{
    try
    {
        Logger::info("Handler::StatByCategory , Record statByCategory request received");
        auto userId = authCheck(_req, _res);
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

        Logger::info("Handler::StatByCategory , Record statByCategory request processed successfully");
        Handler::sendSuccess(_res, result, "record statByCategory retrieved successfully");
    }
    catch (const std::exception &e)
    {
        Handler::handleInternalError(_req, _res, e);
    }
}

void Handler::Filter(RecordDao &_dao, const Request &_req, Response &_res)
{
    try
    {
        Logger::info("Handler::Filter , Record filter request received");
        auto userId = authCheck(_req, _res);
        if (userId == -1)
        {
            return;
        }

        Limit l;
        ValidationResult vr = Validator::validateLimit(_req, l);
        if (!vr.is_valid)
        {
            Logger::error("Handler::Filter , Record filter request has invalid parameters: " + vr.message);
            Handler::sendError(_res, HttpStatus::bad_request, vr.code, vr.message);
            return;
        }

        std::string keyword = _req.get_param_value("keyword");
        std::string month = _req.get_param_value("month");
        int count = _dao.count_records(userId, month, keyword);
        Json records_json = Json::array();

        for (auto &r : _dao.filter(keyword, month, userId, l))
        {
            Json item;
            RecordToJson(r, item);
            records_json.push_back(item);
        }

        Json res_json;
        res_json["total"] = count;
        res_json["page"] = l.page;
        res_json["pageSize"] = l.pageSize;
        res_json["records"] = records_json;

        Logger::info("Handler::Filter , Record filter request processed successfully");
        Handler::sendSuccess(_res, res_json, "record filter retrieved successfully");
    }
    catch (const std::exception &e)
    {
        Handler::handleInternalError(_req, _res, e);
    }
}

void Handler::Update(RecordDao &_dao, const Request &_req, Response &_res)
{
    try
    {
        Logger::info("Handler::Update , Record update request received");
        auto userId = authCheck(_req, _res);
        if (userId == -1)
        {
            return;
        }

        auto idIt = _req.params.find("id");
        if (idIt == _req.params.end())
        {
            Logger::error("Handler::Update , Record update request missing id parameter");
            Handler::sendError(_res, HttpStatus::bad_request, BusinessStatus::InvalidPARAM, "missing id parameter");
            return;
        }

        int id = 0;
        ValidationResult vr = Validator::parsePositiveInt(idIt->second, id);
        if (!vr.is_valid)
        {
            Logger::error("Handler::Update , Record update request has invalid id parameter: " + idIt->second);
            Handler::sendError(_res, HttpStatus::bad_request, vr.code, vr.message);
            return;
        }

        auto j = Json::parse(_req.body);
        vr = Validator::validateRecordJson(j);
        if (!vr.is_valid)
        {
            Logger::error("Handler::Update , Record update request has invalid JSON parameters: " + vr.message);
            Handler::sendError(_res, HttpStatus::bad_request, vr.code, vr.message);
            return;
        }

        Record r;
        JsonToRecord(j, r);
        r.user_id = userId;

        if (_dao.update(id, r, userId))
        {
            Logger::info("Handler::Update , Record update request processed successfully");
            Handler::sendSuccess(_res, Json::object(), "record updated successfully");
        }
        else
        {
            Logger::info("Handler::Update , Record update request processed with failure");
            Handler::sendError(_res, HttpStatus::bad_request, BusinessStatus::InvalidPARAM, "failed to update record");
        }
    }
    catch (const std::exception &e)
    {
        Handler::handleInternalError(_req, _res, e);
    }
}

void Handler::Remove(RecordDao &_dao, const Request &_req, Response &_res)
{
    try
    {
        Logger::info("Handler::Remove , Record remove request received");
        auto userId = authCheck(_req, _res);
        if (userId == -1)
        {
            return;
        }

        auto idIt = _req.params.find("id");
        if (idIt == _req.params.end())
        {
            Logger::error("Handler::Remove , Record remove request missing id parameter");
            Handler::sendError(_res, HttpStatus::bad_request, BusinessStatus::InvalidPARAM, "missing id parameter");
            return;
        }

        int id = 0;
        ValidationResult vr = Validator::parsePositiveInt(idIt->second, id);
        if (!vr.is_valid)
        {
            Logger::error("Handler::Remove , Record remove request has invalid id parameter");
            Handler::sendError(_res, HttpStatus::bad_request, vr.code, vr.message);
            return;
        }

        Logger::info("Handler::Remove , Record remove id param: " + std::to_string(id));
        if (_dao.remove(id, userId))
        {
            Logger::info("Handler::Remove , Record remove request processed successfully");
            Handler::sendSuccess(_res, Json::object(), "record deleted successfully");
        }
        else
        {
            Logger::info("Handler::Remove , Record remove request processed with failure");
            Handler::sendError(_res, HttpStatus::bad_request, BusinessStatus::InvalidPARAM, "failed to delete record");
        }
    }
    catch (const std::exception &e)
    {
        Handler::handleInternalError(_req, _res, e);
    }
}

void Handler::Export(RecordDao &_dao, const Request &_req, Response &_res)
{
    try
    {
        Logger::info("Handler::Export , Record export request received");
        auto userId = authCheck(_req, _res);
        if (userId == -1)
        {
            return;
        }

        Logger::info("Handler::Export , Record export user_id: " + std::to_string(userId));

        auto records = _dao.list_order_by_id(userId);

        Logger::info("Handler::Export , Record export records: " + std::to_string(records.size()));

        std::ostringstream oss;
        oss << "\xEF\xBB\xBF";
        oss << "id,金额,备注,类型,日期,分类\n";

        for (const auto &r : records)
        {
            oss << r.id << "," << r.amount << ",\"" << r.note << "\"," << r.type << "," << r.time << "," << r.category << "\n";
        }

        Logger::info("Handler::Export , Record export records size: " + std::to_string(records.size()));

        _res.set_content(oss.str(), "text/csv");
        _res.set_header("Content-Disposition", "attachment; filename=\"records.csv\"");
    }
    catch (const std::exception &e)
    {
        Handler::handleInternalError(_req, _res, e);
    }
}

void Handler::Add(UserDao &_dao, const Request &_req, Response &_res)
{
    try
    {
        Logger::info("Handler::Add , User Add request received");
        if (!Json::accept(_req.body))
        {
            Logger::error("Handler::Add , User Add request body is not valid JSON");
            Handler::sendError(_res, HttpStatus::bad_request, BusinessStatus::InvalidJSON, "invalid JSON");
            return;
        }

        auto j = Json::parse(_req.body);
        ValidationResult vr = Validator::validateUserJson(j);
        if (!vr.is_valid)
        {
            Logger::error("Handler::Add , User Add request has invalid JSON parameters: " + vr.message);
            Handler::sendError(_res, HttpStatus::bad_request, vr.code, vr.message);
            return;
        }

        User user;
        JsonToUser(j, user);
        user.password = Crypto::sha256(user.password);

        if (!_dao.add(user))
        {
            Logger::error("Handler::Add , User Add request processed failed");
            Handler::sendError(_res, HttpStatus::internal_error, BusinessStatus::InternalError, "failed to add user");
            return;
        }
        Logger::info("Handler::Add , User Add request processed successfully");
        Handler::sendSuccess(_res, Json::object(), "user added successfully");
    }
    catch (const std::exception &e)
    {
        Handler::handleInternalError(_req, _res, e);
    }
}

void Handler::Update(UserDao &_dao, const Request &_req, Response &_res)
{
    try
    {
        Logger::info("Handler::Update , User Update request received");
        if (!Json::accept(_req.body))
        {
            Logger::error("Handler::Update , User Update request body is not valid JSON");
            Handler::sendError(_res, HttpStatus::bad_request, BusinessStatus::InvalidJSON, "invalid JSON");
            return;
        }
        auto j = Json::parse(_req.body);
        ValidationResult vr = Validator::validateUserJson(j);
        if (!vr.is_valid)
        {
            Logger::error("Handler::Update , User Update request has invalid JSON parameters: " + vr.message);
            Handler::sendError(_res, HttpStatus::bad_request, vr.code, vr.message);
            return;
        }

        User user;
        JsonToUser(j, user);
        user.password = Crypto::sha256(user.password);

        if (!_dao.update(user))
        {
            Logger::error("Handler::Update , User Update request processed failed");
            Handler::sendError(_res, HttpStatus::internal_error, BusinessStatus::InternalError, "failed to update user");
            return;
        }
        Logger::info("Handler::Update , User Update request processed successfully");
        Handler::sendSuccess(_res, Json::object(), "user updated successfully");
    }
    catch (const std::exception &e)
    {
        Handler::handleInternalError(_req, _res, e);
    }
}

void Handler::Remove(UserDao &_dao, const Request &_req, Response &_res)
{
    try
    {
        Logger::info("Handler::Remove , User Remove request received");
        if (_req.params.find("username") == _req.params.end())
        {
            Logger::error("Handler::Remove , User Remove request missing username parameter");
            Handler::sendError(_res, HttpStatus::bad_request, BusinessStatus::InvalidPARAM, "missing username parameter");
            return;
        }

        std::string username = _req.get_param_value("username");
        if (username.empty())
        {
            Logger::error("Handler::Remove , User Remove request has empty username parameter");
            Handler::sendError(_res, HttpStatus::bad_request, BusinessStatus::InvalidPARAM, "invalid username parameter");
            return;
        }

        if (!_dao.remove(username))
        {
            Logger::error("Handler::Remove , User Remove request processed failed");
            Handler::sendError(_res, HttpStatus::internal_error, BusinessStatus::InternalError, "failed to delete user");
            return;
        }
        Logger::info("Handler::Remove , User Remove request processed successfully");
        Handler::sendSuccess(_res, Json::object(), "user deleted successfully");
    }
    catch (const std::exception &e)
    {
        Handler::handleInternalError(_req, _res, e);
    }
}

void Handler::Login(UserDao &_dao, const Request &_req, Response &_res)
{
    try
    {
        Logger::info("Handler::Login , User Login request received");

        if (!Json::accept(_req.body))
        {
            Logger::error("Handler::Login , User Login request body is not valid JSON");
            Handler::sendError(_res, HttpStatus::bad_request, BusinessStatus::InvalidJSON, "invalid JSON");
            return;
        }

        auto j = Json::parse(_req.body);
        ValidationResult vr = Validator::validateUserJson(j);
        if (!vr.is_valid)
        {
            Logger::error("Handler::Login , User Login request has invalid JSON parameters: " + vr.message);
            Handler::sendError(_res, HttpStatus::bad_request, vr.code, vr.message);
            return;
        }

        User user;
        JsonToUser(j, user);
        auto userOptional = _dao.query(user.username);
        auto hashedPassword = Crypto::sha256(user.password);

        if (!userOptional.has_value() || userOptional.value().password != hashedPassword)
        {
            Handler::sendError(_res, HttpStatus::unauthorized, BusinessStatus::Unauthorized, "invalid username or password");
            Logger::error("Handler::Login , User Login failed");
            return;
        }
        Json data;
        data["token"] = TokenManager::generate_token(userOptional.value().id);
        Handler::sendSuccess(_res, data, "login success");
        Logger::info("Handler::Login , User Login success");
    }
    catch (const std::exception &e)
    {
        Handler::handleInternalError(_req, _res, e);
    }
}

void Handler::Register(UserDao &_dao, const Request &_req, Response &_res)
{
    try
    {
        Logger::info("Handler::Register , User Register request received");
        if (!Json::accept(_req.body))
        {
            Logger::error("Handler::Register , User Register request body is not valid JSON");
            Handler::sendError(_res, HttpStatus::bad_request, BusinessStatus::InvalidJSON, "invalid JSON");
            return;
        }

        auto j = Json::parse(_req.body);
        ValidationResult vr = Validator::validateUserJson(j);
        if (!vr.is_valid)
        {
            Logger::error("Handler::Register , User Register request has invalid JSON parameters: " + vr.message);
            Handler::sendError(_res, HttpStatus::bad_request, vr.code, vr.message);
            return;
        }

        User user;
        JsonToUser(j, user);
        auto userOptional = _dao.query(user.username);

        if (userOptional.has_value())
        {
            Logger::error("Handler::Register , User Register failed");
            Handler::sendError(_res, HttpStatus::bad_request, BusinessStatus::InvalidPARAM, "username already exists");
            return;
        }
        user.password = Crypto::sha256(user.password);
        _dao.add(user);
        Logger::info("Handler::Register , User Register success");
        Handler::sendSuccess(_res, Json::object(), "user registered successfully");
    }
    catch (const std::exception &e)
    {
        Handler::handleInternalError(_req, _res, e);
    }
}

void Handler::Logout(const Request &_req, Response &_res)
{
    try
    {
        Logger::info("Handler::Logout , User Logout request received");
        auto token = _req.get_header_value("token");
        if (TokenManager::validate_token(token) != -1)
        {
            TokenManager::remove_token(token);
            Logger::info("Handler::Logout , User Logout success");
            Handler::sendSuccess(_res, Json::object(), "logout success");
        }
        else
        {
            Logger::info("Handler::Logout , User Logout failed");
            Handler::sendError(_res, HttpStatus::unauthorized, BusinessStatus::Unauthorized, "invalid token");
        }
    }
    catch (const std::exception &e)
    {
        Handler::handleInternalError(_req, _res, e);
    }
}

int Handler::authCheck(const Request &_req, Response &_res)
{
    auto token = _req.get_header_value("token");
    auto userId = TokenManager::validate_token(token);
    if (userId != -1)
    {
        Logger::info("Handler::authCheck , User authCheck success, userId: " + std::to_string(userId));
        return userId;
    }

    Logger::info("Handler::authCheck , User authCheck failed, token is invalid");
    Handler::sendError(_res, HttpStatus::unauthorized, BusinessStatus::Unauthorized, "invalid token");
    return -1;
}

void Handler::handleInternalError(const Request &_req, Response &_res, const std::exception &e)
{
    Logger::error("Location : " + _req.path + " , Exception: " + e.what());
    Handler::sendError(_res, HttpStatus::internal_error, BusinessStatus::InternalError, "internal server error");
}

void Handler::JsonToRecord(Json &_j, Record &_r)
{
    _r.amount = _j["amount"];
    _r.note = _j["note"];
    _r.type = _j["type"];
    _r.time = _j["time"];
    _r.category = _j["category"];
}

void Handler::RecordToJson(const Record &_r, Json &_j)
{
    _j["id"] = _r.id;
    _j["amount"] = _r.amount;
    _j["note"] = _r.note;
    _j["type"] = _r.type;
    _j["time"] = _r.time;
    _j["category"] = _r.category;
}

void Handler::JsonToUser(Json &_j, User &_u)
{
    _u.username = _j["username"];
    _u.password = _j["password"];
}

void Handler::sendSuccess(Response &_res, const Json &_data, const std::string &_message)
{
    Handler::Json result;
    result["code"] = 0;
    result["message"] = _message;
    result["data"] = _data;
    _res.set_content(result.dump(), "application/json");
    _res.status = 200;
}

void Handler::sendError(Response &_res, HttpStatus status, BusinessStatus code, const std::string &_message)
{
    Handler::Json result;
    result["code"] = static_cast<int>(code);
    result["message"] = _message;
    result["data"] = nullptr;
    _res.set_content(result.dump(), "application/json");
    _res.status = static_cast<int>(status);
}
