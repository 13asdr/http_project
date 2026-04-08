#include "handler.h"
#include "Logger.h"

void Handler::Add(RecordDao &dao, const httplib::Request &req, httplib::Response &res)
{
    try
    {
        Logger::info("accept add request body: " + req.body);
        auto j = nlohmann::json::parse(req.body);

        // 解析JSON
        Record r;
        JsonToRecord(j, r);

        // std::cout << "category: " << r.category << std::endl;
        nlohmann::json result;
        if (dao.add(r))
        {
            result["status"] = "ok";
            result["message"] = "record added successfully";
        }
        else
        {
            result["status"] = "error";
            result["message"] = "add failed";
        }
        res.set_content(result.dump(), "application/json");
    }
    catch (const std::exception &e)
    {
        std::string str_exception = e.what();
        Logger::error("Location : " + req.path + " , Exception: " + str_exception); // path是record/add
        res.status = 500;
        res.set_content("internal server error", "text/plain");
    }
}

void Handler::List(RecordDao &dao, const httplib::Request &req, httplib::Response &res)
{
    try
    {
        Logger::info("accept list request");
        auto records = dao.list();

        nlohmann::json result = nlohmann::json::array();
        for (auto &r : records)
        {
            nlohmann::json item;
            RecordToJson(r, item);
            result.push_back(item);
        }
        res.set_content(result.dump(), "application/json");
    }
    catch (const std::exception &e)
    {
        std::string str_exception = e.what();
        Logger::error("Location : " + req.path + " , Exception: " + str_exception); // path是record/list
        res.status = 500;
        res.set_content("internal server error", "text/plain");
    }
};

void Handler::StatByCategory(RecordDao &dao, const httplib::Request &req, httplib::Response &res)
{
    try
    {
        Logger::info("accept statByCategory request");
        auto records = dao.statByCategory();

        nlohmann::json result = nlohmann::json::array(); // result 是一个数组
        for (const auto &[key, value] : records)
        {
            nlohmann::json item;
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
        res.set_content("internal server error", "text/plain");
    }
}

void Handler::ListByMonth(RecordDao &dao, const httplib::Request &req, httplib::Response &res)
{
    try
    {
        Logger::info("accept listByMonth request");
        std::string month = req.get_param_value("month");

        nlohmann::json result = nlohmann::json::array();
        for (auto &r : dao.listByMonth(month))
        {
            nlohmann::json item;
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
        res.set_content("internal server error", "text/plain");
    }
}

void Handler::Search(RecordDao &dao, const httplib::Request &req, httplib::Response &res)
{
    try
    {
        Logger::info("accept search request");
        std::string keyword = req.get_param_value("keyword");
        // http://localhost:8080/record/search?keyword=吃 param是keyword , value是吃 ,search是调用函数

        nlohmann::json result = nlohmann::json::array();
        for (auto &r : dao.search(keyword))
        {
            nlohmann::json item;
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
        res.set_content("internal server error", "text/plain");
    }
}

void Handler::Filter(RecordDao &dao, const httplib::Request &req, httplib::Response &res)
{
    try
    {
        Logger::info("accept filter request");
        std::string keyword = req.get_param_value("keyword");
        std::string month = req.get_param_value("month");

        nlohmann::json result = nlohmann::json::array();
        for (auto &r : dao.filter(keyword, month))
        {
            nlohmann::json item;
            RecordToJson(r, item);
            result.push_back(item);
        }
        res.set_content(result.dump(), "application/json");
    }
    catch (const std::exception &e)
    {
        std::string str_exception = e.what();
        Logger::error("Location : " + req.path + " , Exception: " + str_exception); // path是record/filter
        res.status = 500;
        res.set_content("internal server error", "text/plain");
    }
}

void Handler::Update(RecordDao &dao, const httplib::Request &req, httplib::Response &res)
{
    try
    {
        Logger::info("accept update request body: " + req.body);
        int id = std::stoi(req.get_param_value("id"));
        auto j = nlohmann::json::parse(req.body); // parse ,将字符串转换成对象

        Record r;
        JsonToRecord(j, r);

        nlohmann::json result; // 前后端都返回JSON对象
        if (dao.update(id, r))
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
        res.set_content("internal server error", "text/plain");
    }
}

void Handler::Remove(RecordDao &dao, const httplib::Request &req, httplib::Response &res)
{
    try
    {
        Logger::info("accept remove request");
        int id = std::stoi(req.get_param_value("id"));

        nlohmann::json result;
        if (dao.remove(id))
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
        res.set_content("internal server error", "text/plain");
    }
}

void Handler::JsonToRecord(nlohmann::json &j, Record &r)
{
    r.amount = j["amount"];
    r.note = j["note"];
    r.type = j["type"];
    r.time = j["time"];
    r.category = j["category"];
}

void Handler::RecordToJson(const Record &r, nlohmann::json &j)
{
    j["amount"] = r.amount;
    j["note"] = r.note;
    j["type"] = r.type;
    j["time"] = r.time;
    j["category"] = r.category;
}
