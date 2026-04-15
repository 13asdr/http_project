#include "handler.h"
#include "Logger.h"

void Handler::Add(RecordDao &dao, const Request &req, Response &res)
{
    try
    {
        // 打印日志
        Logger::info("Handler::Add RecordAdd request received"); // 不要打印body参数

        // 处理body参数
        if (!Json::accept(req.body))
        {
            Logger::error("Handler::Add RecordAdd request body is not valid JSON");
            res.status = 400;
            res.set_content("invalid JSON", "text/plain");
            return;
        }
        auto j = Json::parse(req.body);

        auto userId = authCheck(req, res);
        if (userId == -1)
        {
            return;
        }

        // 解析JSON
        Record r;
        JsonToRecord(j, r);
        r.user_id = userId;

        // 执行添加操作然后返回结果
        Json res_json;
        if (dao.add(r))
        {
            res_json["status"] = "ok";
            res_json["message"] = "record added successfully";
        }
        else
        {
            res_json["status"] = "error";
            res_json["message"] = "add failed";
        }
        res.set_content(res_json.dump(), "application/json");
    }
    catch (const std::exception &e)
    {
        std::string str_exception = e.what();
        Logger::error("Location : " + req.path + " , Exception: " + str_exception); // path是record/add
        res.status = 500;
        res.set_content("internal server Record error", "text/plain");
    }
}

void Handler::List(RecordDao &dao, const Request &req, Response &res)
{
    try
    {
        Logger::info("Handler::List , Record list request received");

        // 获取user_id
        auto userId = authCheck(req, res);
        if (userId == -1)
        {
            return;
        }
        // 获取分页参数
        limit l = Handler::getLimit(req);

        // 查询记录条数
        int count = dao.count_records(userId);
        // 查询记录
        auto records = dao.list_order_by_time(userId, l);
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
        res.set_content(res_json.dump(), "application/json");
    }
    catch (const std::exception &e)
    {
        std::string str_exception = e.what();
        Logger::error("Location : " + req.path + " , Exception: " + str_exception); // path是record/list
        res.status = 500;
        res.set_content("Handler::List , Record list error", "text/plain");
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

        Json result = Json::array(); // result 是一个数组
        for (const auto &[key, value] : records)
        {
            Json item;
            item["category"] = key;
            item["total"] = value;
            result.push_back(item);
        }
        res.set_content(result.dump(), "application/json");
    }
    catch (const std::exception &e)
    {
        std::string str_exception = e.what();
        Logger::error("Location : " + req.path + " , Exception: " + str_exception); // path是record/statByCategory
        res.status = 500;
        res.set_content("Handler::StatByCategory , Record statByCategory error", "text/plain");
    }
}

void Handler::ListByMonth(RecordDao &dao, const Request &req, Response &res)
{
    try
    {
        Logger::info("Handler::ListByMonth , Record listByMonth request received");

        // 获取user_id
        auto userId = authCheck(req, res);
        if (userId == -1)
        {
            return;
        }

        std::string month = req.get_param_value("month");
        limit l = Handler::getLimit(req);
        Json result = Json::array();

        for (auto &r : dao.listByMonth(month, userId, l))
        {
            Json item;
            RecordToJson(r, item);
            result.push_back(item);
        }
        res.set_content(result.dump(), "application/json");
    }
    catch (const std::exception &e)
    {
        std::string str_exception = e.what();
        Logger::error("Location : " + req.path + " , Exception: " + str_exception); // path是record/listByMonth
        res.status = 500;
        res.set_content("Handler::ListByMonth , Record listByMonth error", "text/plain");
    }
}

void Handler::Search(RecordDao &dao, const Request &req, Response &res)
{
    try
    {
        Logger::info("accept Record search request");
        auto userId = authCheck(req, res);
        if (userId == -1)
        {
            return;
        }

        limit l = Handler::getLimit(req);
        std::string keyword = req.get_param_value("keyword");
        // http://localhost:8080/record/search?keyword=吃 param是keyword , value是吃 ,search是调用函数
        Json result = Json::array();

        for (auto &r : dao.search(keyword, userId, l))
        {
            Json item;
            RecordToJson(r, item);
            result.push_back(item);
        }
        res.set_content(result.dump(), "application/json");
    }
    catch (const std::exception &e)
    {
        std::string str_exception = e.what();
        Logger::error("Location : " + req.path + " , Exception: " + str_exception); // path是record/search
        res.status = 500;
        res.set_content("Handler::Search , Record search error", "text/plain");
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
        limit l = Handler::getLimit(req);

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

        // 构建响应JSON
        Json res_json;
        res_json["total"] = count;
        res_json["page"] = l.page;
        res_json["pageSize"] = l.pageSize;
        res_json["records"] = records_json;

        res.set_content(res_json.dump(), "application/json");
    }
    catch (const std::exception &e)
    {
        std::string str_exception = e.what();
        Logger::error("Location : " + req.path + " , Exception: " + str_exception); // path是record/filter
        res.status = 500;
        res.set_content("Handler::Filter , Record filter error", "text/plain");
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

        int id = std::stoi(req.get_param_value("id"));
        auto j = Json::parse(req.body); // parse ,将字符串转换成对象

        Record r;
        JsonToRecord(j, r);
        r.user_id = userId;

        Json result; // 前后端都返回JSON对象
        if (dao.update(id, r, userId))
        {
            result["status"] = "ok";
            result["message"] = "record updated successfully";
        }
        else
        {
            result["status"] = "error";
            result["message"] = "update failed";
        }
        res.set_content(result.dump(), "application/json");
    }
    catch (const std::exception &e)
    {
        std::string str_exception = e.what();
        Logger::error("Location : " + req.path + " , Exception: " + str_exception); // path是record/update
        res.status = 500;
        res.set_content("Handler::Update , Record update error", "text/plain");
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

        std::string idStr = req.get_param_value("id");
        Logger::info("Handler::Remove , Record remove id param: " + idStr);
        int id = std::stoi(idStr);

        Json result;

        if (dao.remove(id, userId))
        {
            result["status"] = "ok";
            result["message"] = "record deleted successfully";
        }
        else
        {
            result["status"] = "error";
            result["message"] = "delete failed";
        }
        res.set_content(result.dump(), "application/json");
    }
    catch (const std::exception &e)
    {
        std::string str_exception = e.what();
        Logger::error("Location : " + req.path + " , Exception: " + str_exception); // path是record/remove
        res.status = 500;
        res.set_content("Handler::Remove , Record remove error", "text/plain");
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
        oss << "\xEF\xBB\xBF"; // UTF-8 BOM，加在第一行前面
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
        std::string str_exception = e.what();
        Logger::error("Location : " + req.path + " , Exception: " + str_exception); // path是record/export
        res.status = 500;
        res.set_content("Handler::Export , Record export error", "text/plain");
    }
}

// 用户接口:
void Handler::Add(UserDao &dao, const Request &req, Response &res)
{
    try
    {
        Logger::info("Handler::Add , Record User Add request body: " + req.body);
        auto j = Json::parse(req.body);
        User user;
        JsonToUser(j, user);

        Json result;
        if (dao.add(user))
        {
            result["status"] = "ok";
            result["message"] = "user added successfully";
        }
        else
        {
            result["status"] = "error";
            result["message"] = "user add failed";
        }
        res.set_content(result.dump(), "application/json");
    }
    catch (const std::exception &e)
    {
        std::string str_exception = e.what();
        Logger::error("Location : " + req.path + " , Exception: " + str_exception); // path是user/add
        res.status = 500;
        res.set_content("internal server User error", "text/plain");
    }
}
void Handler::Update(UserDao &dao, const Request &req, Response &res)
{
    try
    {
        Logger::info("accept User Update request body: " + req.body);
        auto j = Json::parse(req.body);
        User user;
        JsonToUser(j, user);

        Json result;
        if (dao.update(user))
        {
            result["status"] = "ok";
            result["message"] = "user updated successfully";
        }
        else
        {
            result["status"] = "error";
            result["message"] = "user update failed";
        }
        res.set_content(result.dump(), "application/json");
    }
    catch (const std::exception &e)
    {
        std::string str_exception = e.what();
        Logger::error("Location : " + req.path + " , Exception: " + str_exception); // path是user/update
        res.status = 500;
        res.set_content("internal server User error", "text/plain");
    }
}

void Handler::Remove(UserDao &dao, const Request &req, Response &res)
{
    try
    {
        Logger::info("accept User Remove request");
        std::string username = req.get_param_value("username");

        Json result;
        if (dao.remove(username))
        {
            result["status"] = "ok";
            result["message"] = "user deleted successfully";
        }
        else
        {
            result["status"] = "error";
            result["message"] = "delete failed";
        }
        res.set_content(result.dump(), "application/json");
    }
    catch (const std::exception &e)
    {
        std::string str_exception = e.what();
        Logger::error("Location : " + req.path + " , Exception: " + str_exception); // path是user/remove
        res.status = 500;
        res.set_content("internal server User error", "text/plain");
    }
}

void Handler::Login(UserDao &dao, const Request &req, Response &res)
{
    try
    {
        // 打印日志
        Logger::info("Handler::Login , User Login request received");

        // 处理body参数
        if (!Json::accept(req.body))
        {
            Logger::error("Handler::Login , User Login request body is not valid JSON");
            res.status = 400;
            res.set_content("invalid JSON", "text/plain");
            return;
        }
        auto j = Json::parse(req.body);

        User user;
        JsonToUser(j, user);
        auto userOptional = dao.query(user.username);

        Json result;
        if (!userOptional.has_value() || userOptional.value().password != user.password)
        {
            result["status"] = "error";
            result["message"] = "user not found or password error";
            result["token"] = "";
            Logger::error("Handler::Login , User Login failed");
        }
        else
        {
            result["status"] = "ok";
            result["message"] = "login success";
            result["token"] = TokenManager::generate_token(userOptional.value().id);
            Logger::info("Handler::Login , User Login success");
        }
        res.set_content(result.dump(), "application/json");
    }
    catch (const std::exception &e)
    {
        std::string str_exception = e.what();
        Logger::error("Location : " + req.path + " , Exception: " + str_exception); // path是user/login
        res.status = 500;
        res.set_content("Handler::Login , User Login error", "text/plain");
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
            res.status = 400;
            res.set_content("invalid JSON", "text/plain");
            return;
        }
        auto j = Json::parse(req.body);

        User user;
        auto userOptional = dao.query(user.username);
        JsonToUser(j, user);
        Json result;

        if (userOptional.has_value())
        {
            result["status"] = "error";
            result["message"] = "username already exists";
            Logger::error("Handler::Register , User Register failed");
        }
        else
        {
            dao.add(user);
            result["status"] = "ok";
            result["message"] = "register success , please login";
            Logger::info("Handler::Register , User Register success");
        }
        res.set_content(result.dump(), "application/json");
    }
    catch (const std::exception &e)
    {
        std::string str_exception = e.what();
        Logger::error("Location : " + req.path + " , Exception: " + str_exception); // path是user/register
        res.status = 500;
        res.set_content("Handler::Register , User Register error", "text/plain");
    }
}
void Handler::Logout(const Request &req, Response &res)
{
    try
    {
        Logger::info("Handler::Logout , User Logout request received");
        // 前端传递token,后端验证token,如果token有效,则删除token,返回logout success
        auto token = req.get_header_value("token");
        Json result;
        if (TokenManager::validate_token(token) != -1)
        {
            TokenManager::remove_token(token);
            result["status"] = "ok";
            result["message"] = "logout success";
            Logger::info("Handler::Logout , User Logout success");
        }
        else
        {
            result["status"] = "error";
            result["message"] = "logout failed , token is invalid";
            Logger::info("User Logout failed");
        }
        res.set_content(result.dump(), "application/json");
    }
    catch (const std::exception &e)
    {
        std::string str_exception = e.what();
        Logger::error("Location : " + req.path + " , Exception: " + str_exception); // path是user/logout
        res.status = 500;
        res.set_content("Handler::Logout , User Logout error", "text/plain");
    }
}
int Handler::authCheck(const Request &req, Response &res)
{
    auto token = req.get_header_value("token");
    auto userId = TokenManager::validate_token(token);
    if (userId != -1) // 如果token有效,则返回用户id
    {
        Logger::info("User authCheck success, userId: " + std::to_string(userId));
        return userId;
    }
    else
    {
        Logger::info("User authCheck failed, token is invalid");
        res.status = 401;
        res.set_content("User authCheck failed , unauthorized", "text/plain");
        return -1;
    }
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
    u.password = Crypto::sha256(j["password"]);
}

// void Handler::UserToJson(const User &u, Json &j)
// {
//     j["username"] = u.username;
//     j["password"] = u.password;
// }

limit Handler::getLimit(const Request &req)
{
    int _page = req.has_param("page") ? std::stoi(req.get_param_value("page")) : 1;
    int _pageSize = req.has_param("pageSize") ? std::stoi(req.get_param_value("pageSize")) : 10;
    return {_page, _pageSize};
}
