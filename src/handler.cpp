#include "handler.h"
#include "Logger.h"

void Handler::Add(RecordDao &dao, const Request &req, Response &res)
{
    try
    {
        Logger::info("accept add request body: " + req.body);
        auto j = Json::parse(req.body);

        // 解析JSON
        Record r;
        JsonToRecord(j, r);

        // std::cout << "category: " << r.category << std::endl;
        Json result;
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

void Handler::List(RecordDao &dao, const Request &req, Response &res)
{
    try
    {
        Logger::info("accept list request");
        auto records = dao.list_order_by_time();

        Json result = Json::array();
        for (auto &r : records)
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
        Logger::error("Location : " + req.path + " , Exception: " + str_exception); // path是record/list
        res.status = 500;
        res.set_content("internal server error", "text/plain");
    }
};

void Handler::StatByCategory(RecordDao &dao, const Request &req, Response &res)
{
    try
    {
        Logger::info("accept statByCategory request");
        auto records = dao.statByCategory();

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
        res.set_content("internal server error", "text/plain");
    }
}

void Handler::ListByMonth(RecordDao &dao, const Request &req, Response &res)
{
    try
    {
        Logger::info("accept listByMonth request");
        std::string month = req.get_param_value("month");

        Json result = Json::array();
        for (auto &r : dao.listByMonth(month))
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
        res.set_content("internal server error", "text/plain");
    }
}

void Handler::Search(RecordDao &dao, const Request &req, Response &res)
{
    try
    {
        Logger::info("accept search request");
        std::string keyword = req.get_param_value("keyword");
        // http://localhost:8080/record/search?keyword=吃 param是keyword , value是吃 ,search是调用函数

        Json result = Json::array();
        for (auto &r : dao.search(keyword))
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
        res.set_content("internal server error", "text/plain");
    }
}

void Handler::Filter(RecordDao &dao, const Request &req, Response &res)
{
    try
    {
        Logger::info("accept filter request");
        std::string keyword = req.get_param_value("keyword");
        std::string month = req.get_param_value("month");

        Json result = Json::array();
        for (auto &r : dao.filter(keyword, month))
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
        Logger::error("Location : " + req.path + " , Exception: " + str_exception); // path是record/filter
        res.status = 500;
        res.set_content("internal server error", "text/plain");
    }
}

void Handler::Update(RecordDao &dao, const Request &req, Response &res)
{
    try
    {
        Logger::info("accept update request body: " + req.body);
        int id = std::stoi(req.get_param_value("id"));
        auto j = Json::parse(req.body); // parse ,将字符串转换成对象

        Record r;
        JsonToRecord(j, r);

        Json result; // 前后端都返回JSON对象
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

void Handler::Remove(RecordDao &dao, const Request &req, Response &res)
{
    try
    {
        Logger::info("accept remove request");
        int id = std::stoi(req.get_param_value("id"));

        Json result;
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

void Handler::Export(RecordDao &dao, const Request &req, Response &res)
{
    try
    {
        Logger::info("accept export request");
        auto records = dao.list_order_by_id();

        std::ostringstream oss;
        oss << "\xEF\xBB\xBF"; // UTF-8 BOM，加在第一行前面
        oss << "id,金额,备注,类型,日期,分类\n";

        for (const auto &r : records)
        {
            oss << r.id << "," << r.amount << "," << r.note << "," << r.type << "," << r.time << "," << r.category << "\n";
        }

        Logger::info("export records: " + oss.str());

        res.set_content(oss.str(), "text/plain");
        res.set_header("Content-Disposition", "attachment; filename=\"records.csv\"");
    }
    catch (const std::exception &e)
    {
        std::string str_exception = e.what();
        Logger::error("Location : " + req.path + " , Exception: " + str_exception); // path是record/export
        res.status = 500;
        res.set_content("internal server error", "text/plain");
    }
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
    j["amount"] = r.amount;
    j["note"] = r.note;
    j["type"] = r.type;
    j["time"] = r.time;
    j["category"] = r.category;
}
