#include "handler.h"
#include "Logger.h"

void Handler::Add(RecordDao &dao, const Request &req, Response &res)
{
    try
    {
        Logger::info("Handler::Add , RecordAdd request received");

        if (!Json::accept(req.body))
        {
            Logger::error("Handler::Add , RecordAdd request body is not valid JSON");
            Handler::sendError(res, http_status::bad_request, message_code::InvalidJSON, "invalid JSON");
            return;
        }

        auto j = Json::parse(req.body);
        if (!Validator::validateRecordJson(j, res))
        {
            return;
        }

        auto userId = authCheck(req, res);
        if (userId == -1)
        {
            return;
        }

        Record r;
        JsonToRecord(j, r);
        r.user_id = userId;

        if (dao.add(r))
        {
            Logger::info("Handler::Add , RecordAdd request processed successfully");
            Handler::sendSuccess(res, Json::object(), "record added successfully");
        }
        else
        {
            Logger::error("Handler::Add , RecordAdd request failed");
            Handler::sendError(res, http_status::internal_error, message_code::InternalError, "Failed to add record");
        }
    }
    catch (const std::exception &e)
    {
        Handler::handleInternalError(req, res, e);
    }
}

void Handler::List(RecordDao &dao, const Request &req, Response &res)
{
    try
    {
        Logger::info("Handler::List , Record list request received");

        auto userId = authCheck(req, res);
        if (userId == -1)
        {
            return;
        }

        limit l;
        if (!Validator::validateLimit(req, res, l))
        {
            return;
        }

        int count = dao.count_records(userId);
        auto records = dao.list_order_by_timeAndId(userId, l);
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
        Handler::sendSuccess(res, res_json, "record list retrieved successfully");
    }
    catch (const std::exception &e)
    {
        Handler::handleInternalError(req, res, e);
    }
};

void Handler::StatByCategory(RecordDao &dao, const Request &req, Response &res)
{
    try
    {
        Logger::info("Handler::StatByCategory , Record statByCategory request received");
        auto userId = authCheck(req, res);
        if (userId == -1)
        {
            return;
        }

        auto records = dao.statByCategory(userId);

        Json result = Json::array();
        for (const auto &[key, value] : records)
        {
            Json item;
            item["category"] = key;
            item["total"] = value;
            result.push_back(item);
        }

        Logger::info("Handler::StatByCategory , Record statByCategory request processed successfully");
        Handler::sendSuccess(res, result, "record statByCategory retrieved successfully");
    }
    catch (const std::exception &e)
    {
        Handler::handleInternalError(req, res, e);
    }
}

void Handler::Filter(RecordDao &dao, const Request &req, Response &res)
{
    try
    {
        Logger::info("Handler::Filter , Record filter request received");
        auto userId = authCheck(req, res);
        if (userId == -1)
        {
            return;
        }

        limit l;
        if (!Validator::validateLimit(req, res, l))
        {
            return;
        }

        std::string keyword = req.get_param_value("keyword");
        std::string month = req.get_param_value("month");
        int count = dao.count_records(userId, month, keyword);
        Json records_json = Json::array();

        for (auto &r : dao.filter(keyword, month, userId, l))
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
        Handler::sendSuccess(res, res_json, "record filter retrieved successfully");
    }
    catch (const std::exception &e)
    {
        Handler::handleInternalError(req, res, e);
    }
}

void Handler::Update(RecordDao &dao, const Request &req, Response &res)
{
    try
    {
        Logger::info("Handler::Update , Record update request received");
        auto userId = authCheck(req, res);
        if (userId == -1)
        {
            return;
        }

        auto idIt = req.params.find("id");
        if (idIt == req.params.end())
        {
            Logger::error("Handler::Update , Record update request missing id parameter");
            Handler::sendError(res, http_status::bad_request, message_code::InvalidPARAM, "missing id parameter");
            return;
        }

        int id = 0;
        if (!Validator::parsePositiveInt(idIt->second, id))
        {
            Logger::error("Handler::Update , Record update request has invalid id parameter: " + idIt->second);
            Handler::sendError(res, http_status::bad_request, message_code::InvalidPARAM, "invalid id parameter");
            return;
        }

        auto j = Json::parse(req.body);
        if (!Validator::validateRecordJson(j, res))
        {
            return;
        }

        Record r;
        JsonToRecord(j, r);
        r.user_id = userId;

        if (dao.update(id, r, userId))
        {
            Logger::info("Handler::Update , Record update request processed successfully");
            Handler::sendSuccess(res, Json::object(), "record updated successfully");
        }
        else
        {
            Logger::info("Handler::Update , Record update request processed with failure");
            Handler::sendError(res, http_status::bad_request, message_code::InvalidPARAM, "failed to update record");
        }
    }
    catch (const std::exception &e)
    {
        Handler::handleInternalError(req, res, e);
    }
}

void Handler::Remove(RecordDao &dao, const Request &req, Response &res)
{
    try
    {
        Logger::info("Handler::Remove , Record remove request received");
        auto userId = authCheck(req, res);
        if (userId == -1)
        {
            return;
        }

        auto idIt = req.params.find("id");
        if (idIt == req.params.end())
        {
            Logger::error("Handler::Remove , Record remove request missing id parameter");
            Handler::sendError(res, http_status::bad_request, message_code::InvalidPARAM, "missing id parameter");
            return;
        }

        int id = 0;
        if (!Validator::parsePositiveInt(idIt->second, id))
        {
            Logger::error("Handler::Remove , Record remove request has invalid id parameter");
            Handler::sendError(res, http_status::bad_request, message_code::InvalidPARAM, "invalid id parameter");
            return;
        }

        Logger::info("Handler::Remove , Record remove id param: " + std::to_string(id));
        if (dao.remove(id, userId))
        {
            Logger::info("Handler::Remove , Record remove request processed successfully");
            Handler::sendSuccess(res, Json::object(), "record deleted successfully");
        }
        else
        {
            Logger::info("Handler::Remove , Record remove request processed with failure");
            Handler::sendError(res, http_status::bad_request, message_code::InvalidPARAM, "failed to delete record");
        }
    }
    catch (const std::exception &e)
    {
        Handler::handleInternalError(req, res, e);
    }
}

void Handler::Export(RecordDao &dao, const Request &req, Response &res)
{
    try
    {
        Logger::info("Handler::Export , Record export request received");
        auto userId = authCheck(req, res);
        if (userId == -1)
        {
            return;
        }

        Logger::info("Handler::Export , Record export user_id: " + std::to_string(userId));

        auto records = dao.list_order_by_id(userId);

        Logger::info("Handler::Export , Record export records: " + std::to_string(records.size()));

        std::ostringstream oss;
        oss << "\xEF\xBB\xBF";
        oss << "id,金额,备注,类型,日期,分类\n";

        for (const auto &r : records)
        {
            oss << r.id << "," << r.amount << ",\"" << r.note << "\"," << r.type << "," << r.time << "," << r.category << "\n";
        }

        Logger::info("Handler::Export , Record export records size: " + std::to_string(records.size()));

        res.set_content(oss.str(), "text/csv");
        res.set_header("Content-Disposition", "attachment; filename=\"records.csv\"");
    }
    catch (const std::exception &e)
    {
        Handler::handleInternalError(req, res, e);
    }
}

void Handler::Add(UserDao &dao, const Request &req, Response &res)
{
    try
    {
        Logger::info("Handler::Add , User Add request received");
        if (!Json::accept(req.body))
        {
            Logger::error("Handler::Add , User Add request body is not valid JSON");
            Handler::sendError(res, http_status::bad_request, message_code::InvalidJSON, "invalid JSON");
            return;
        }

        auto j = Json::parse(req.body);
        if (!Validator::validateUserJson(j, res))
        {
            return;
        }
        User user;
        JsonToUser(j, user);
        user.password = Crypto::sha256(user.password);

        if (!dao.add(user))
        {
            Logger::error("Handler::Add , User Add request processed failed");
            Handler::sendError(res, http_status::internal_error, message_code::InternalError, "failed to add user");
            return;
        }
        Logger::info("Handler::Add , User Add request processed successfully");
        Handler::sendSuccess(res, Json::object(), "user added successfully");
    }
    catch (const std::exception &e)
    {
        Handler::handleInternalError(req, res, e);
    }
}

void Handler::Update(UserDao &dao, const Request &req, Response &res)
{
    try
    {
        Logger::info("Handler::Update , User Update request received");
        if (!Json::accept(req.body))
        {
            Logger::error("Handler::Update , User Update request body is not valid JSON");
            Handler::sendError(res, http_status::bad_request, message_code::InvalidJSON, "invalid JSON");
            return;
        }
        auto j = Json::parse(req.body);
        if (!Validator::validateUserJson(j, res))
        {
            return;
        }
        User user;
        JsonToUser(j, user);
        user.password = Crypto::sha256(user.password);

        if (!dao.update(user))
        {
            Logger::error("Handler::Update , User Update request processed failed");
            Handler::sendError(res, http_status::internal_error, message_code::InternalError, "failed to update user");
            return;
        }
        Logger::info("Handler::Update , User Update request processed successfully");
        Handler::sendSuccess(res, Json::object(), "user updated successfully");
    }
    catch (const std::exception &e)
    {
        Handler::handleInternalError(req, res, e);
    }
}

void Handler::Remove(UserDao &dao, const Request &req, Response &res)
{
    try
    {
        Logger::info("Handler::Remove , User Remove request received");
        if (req.params.find("username") == req.params.end())
        {
            Logger::error("Handler::Remove , User Remove request missing username parameter");
            Handler::sendError(res, http_status::bad_request, message_code::InvalidPARAM, "missing username parameter");
            return;
        }

        std::string username = req.get_param_value("username");
        if (username.empty())
        {
            Logger::error("Handler::Remove , User Remove request has empty username parameter");
            Handler::sendError(res, http_status::bad_request, message_code::InvalidPARAM, "invalid username parameter");
            return;
        }

        if (!dao.remove(username))
        {
            Logger::error("Handler::Remove , User Remove request processed failed");
            Handler::sendError(res, http_status::internal_error, message_code::InternalError, "failed to delete user");
            return;
        }
        Logger::info("Handler::Remove , User Remove request processed successfully");
        Handler::sendSuccess(res, Json::object(), "user deleted successfully");
    }
    catch (const std::exception &e)
    {
        Handler::handleInternalError(req, res, e);
    }
}

void Handler::Login(UserDao &dao, const Request &req, Response &res)
{
    try
    {
        Logger::info("Handler::Login , User Login request received");

        if (!Json::accept(req.body))
        {
            Logger::error("Handler::Login , User Login request body is not valid JSON");
            Handler::sendError(res, http_status::bad_request, message_code::InvalidJSON, "invalid JSON");
            return;
        }

        auto j = Json::parse(req.body);
        if (!Validator::validateUserJson(j, res))
        {
            return;
        }

        User user;
        JsonToUser(j, user);
        auto userOptional = dao.query(user.username);
        auto hashedPassword = Crypto::sha256(user.password);

        if (!userOptional.has_value() || userOptional.value().password != hashedPassword)
        {
            sendError(res, http_status::unauthorized, message_code::Unauthorized, "invalid username or password");
            Logger::error("Handler::Login , User Login failed");
            return;
        }
        Json data;
        data["token"] = TokenManager::generate_token(userOptional.value().id);
        sendSuccess(res, data, "login success");
        Logger::info("Handler::Login , User Login success");
    }
    catch (const std::exception &e)
    {
        Handler::handleInternalError(req, res, e);
    }
}

void Handler::Register(UserDao &dao, const Request &req, Response &res)
{
    try
    {
        Logger::info("Handler::Register , User Register request received");
        if (!Json::accept(req.body))
        {
            Logger::error("Handler::Register , User Register request body is not valid JSON");
            Handler::sendError(res, http_status::bad_request, message_code::InvalidJSON, "invalid JSON");
            return;
        }

        auto j = Json::parse(req.body);
        if (!Validator::validateUserJson(j, res))
        {
            return;
        }

        User user;
        JsonToUser(j, user);
        auto userOptional = dao.query(user.username);

        if (userOptional.has_value())
        {
            Logger::error("Handler::Register , User Register failed");
            Handler::sendError(res, http_status::bad_request, message_code::InvalidPARAM, "username already exists");
            return;
        }
        user.password = Crypto::sha256(user.password);
        dao.add(user);
        Logger::info("Handler::Register , User Register success");
        Handler::sendSuccess(res, Json::object(), "user registered successfully");
    }
    catch (const std::exception &e)
    {
        Handler::handleInternalError(req, res, e);
    }
}

void Handler::Logout(const Request &req, Response &res)
{
    try
    {
        Logger::info("Handler::Logout , User Logout request received");
        auto token = req.get_header_value("token");
        if (TokenManager::validate_token(token) != -1)
        {
            TokenManager::remove_token(token);
            Logger::info("Handler::Logout , User Logout success");
            Handler::sendSuccess(res, Json::object(), "logout success");
        }
        else
        {
            Logger::info("Handler::Logout , User Logout failed");
            Handler::sendError(res, http_status::unauthorized, message_code::Unauthorized, "invalid token");
        }
    }
    catch (const std::exception &e)
    {
        Handler::handleInternalError(req, res, e);
    }
}

int Handler::authCheck(const Request &req, Response &res)
{
    auto token = req.get_header_value("token");
    auto userId = TokenManager::validate_token(token);
    if (userId != -1)
    {
        Logger::info("Handler::authCheck , User authCheck success, userId: " + std::to_string(userId));
        return userId;
    }

    Logger::info("Handler::authCheck , User authCheck failed, token is invalid");
    Handler::sendError(res, http_status::unauthorized, message_code::Unauthorized, "invalid token");
    return -1;
}

void Handler::handleInternalError(const Request &req, Response &res, const std::exception &e)
{
    Logger::error("Location : " + req.path + " , Exception: " + e.what());
    Handler::sendError(res, http_status::internal_error, message_code::InternalError, "internal server error");
}

limit Handler::JsonToLimit(Json &j, limit &l)
{
    std::string page = j["page"];
    std::string pageSize = j["pageSize"];

    l.page = std::stoi(page);
    l.pageSize = std::stoi(pageSize);
    return l;
}

void Handler::JsonToRecord(Json &j, Record &r)
{
    r.amount = j["amount"];
    r.note = j["note"];
    r.type = j["type"];
    r.time = j["time"];
    r.category = j["category"];
}

void Handler::RecordToJson(const Record &r, Json &j)
{
    j["id"] = r.id;
    j["amount"] = r.amount;
    j["note"] = r.note;
    j["type"] = r.type;
    j["time"] = r.time;
    j["category"] = r.category;
}

void Handler::JsonToUser(Json &j, User &u)
{
    u.username = j["username"];
    u.password = j["password"];
}

void Handler::sendSuccess(Response &res, const Json &data, const std::string &message)
{
    Handler::Json result;
    result["code"] = 0;
    result["message"] = message;
    result["data"] = data;
    res.set_content(result.dump(), "application/json");
    res.status = 200;
}

void Handler::sendError(Response &res, http_status status, message_code code, const std::string &message)
{
    Handler::Json result;
    result["code"] = static_cast<int>(code);
    result["message"] = message;
    result["data"] = nullptr;
    res.set_content(result.dump(), "application/json");
    res.status = static_cast<int>(status);
}
