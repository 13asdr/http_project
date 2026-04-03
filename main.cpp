#include "httplib.h"
#include "nlohmann/json.hpp"
#include "RecordDao.h"
#include <iostream>

int main()
{
    httplib::Server server; // 创建HTTP服务器
    DBconnect db;
    RecordDao dao(db);

    // 添加记录 POST /record/add
    server.Post("/record/add", [&dao](const httplib::Request &req, httplib::Response &res)
                {
                    try
                    {
                        std::cout << "accept add request body: " << req.body << std::endl;
                        auto j = nlohmann::json::parse(req.body);

                        // 解析JSON
                        Record r;
                        r.amount = j["amount"];
                        r.note = j["note"];
                        r.type = j["type"];
                        r.time = j["time"];
                        r.category = j["category"];

                        std::cout << "category: " << r.category << std::endl;
                        
                        nlohmann::json result;
                        if (dao.add(r))
                        {
                            result["status"] = "ok";
                            result["message"] = "添加成功";
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
                         std::cout << "exception: " << e.what() << std::endl; 
                         res.status = 500;
                         res.set_content("internal server error", "text/plain");
                    } });

    // 查询所有记录 GET /record/list
    server.Get("/record/list", [&dao](const httplib::Request &req, httplib::Response &res)
               {
        auto records = dao.list();

        nlohmann::json result = nlohmann::json::array();
        for (auto& r : records) {
            nlohmann::json item;
            item["id"]     = r.id;
            item["amount"] = r.amount;
            item["note"]   = r.note;
            item["type"]   = r.type;
            item["time"]   = r.time;
            item["category"]   = r.category;
            result.push_back(item);
        }
        res.set_content(result.dump(), "application/json"); });

    // 查询每个类别别的总金额 GET /record/statByCategory
    server.Get("/record/statByCategory", [&dao](const httplib::Request &req, httplib::Response &res)
               {
        auto records = dao.statByCategory();

        nlohmann::json result = nlohmann::json::array();        //result 是一个数组
        for (const auto&[key,value] : records) {
            nlohmann::json item;
            item["category"]   = key;
            item["total"] = value;
            result.push_back(item);
        }
        res.set_content(result.dump(), "application/json"); });

    // 按月份查询 GET /record/listByMonth?month=2026-04
    server.Get("/record/listByMonth", [&dao](const httplib::Request &req, httplib::Response &res)
               {
    std::string month = req.get_param_value("month");

    nlohmann::json result = nlohmann::json::array();
    for (auto& r : dao.listByMonth(month))
    {
        nlohmann::json item;
        item["id"]     = r.id;
        item["amount"] = r.amount;
        item["note"]   = r.note;
        item["type"]   = r.type;
        item["time"]   = r.time;
        item["category"]   = r.category;
        result.push_back(item);
    }
    res.set_content(result.dump(), "application/json"); });

    // 模糊查询
    server.Get("/record/search", [&dao](const httplib::Request &req, httplib::Response &res)
               {
    std::string keyword = req.get_param_value("keyword");   
    //http://localhost:8080/record/search?keyword=吃 param是keyword , value是吃 ,search是调用函数

    nlohmann::json result = nlohmann::json::array();
    for (auto& r : dao.search(keyword))
    {
        nlohmann::json item;
        item["id"]     = r.id;
        item["amount"] = r.amount;
        item["note"]   = r.note;
        item["type"]   = r.type;
        item["time"]   = r.time;
        item["category"]   = r.category;
        result.push_back(item);
    }
    res.set_content(result.dump(), "application/json"); });

    // 更新函数
    server.Put("/record/update", [&dao](const httplib::Request &req, httplib::Response &res)
               {
        int id = std::stoi(req.get_param_value("id"));  
        auto j = nlohmann::json::parse(req.body);   // parse ,将字符串转换成对象
        
        Record r;
        r.amount = j["amount"];
        r.note = j["note"];
        r.type = j["type"];
        r.time = j["time"];
        r.category = j["category"];

        nlohmann::json result;      //前后端都返回JSON对象
        if (dao.update(id, r)) {
            result["status"]  = "ok";
            result["message"] = "更新成功";
        } else {
            result["status"]  = "error";
            result["message"] = "更新失败";
        }
        res.set_content(result.dump(), "application/json"); });

    // 删除记录 DELETE /record/delete?id=1
    server.Delete("/record/delete", [&dao](const httplib::Request &req, httplib::Response &res)
                  {
        int id = std::stoi(req.get_param_value("id"));

        nlohmann::json result;
        if (dao.remove(id)) {
            result["status"]  = "ok";
            result["message"] = "删除成功";
        } else {
            result["status"]  = "error";
            result["message"] = "删除失败";
        }
        res.set_content(result.dump(), "application/json"); });

    // 允许所有跨域请求
    server.set_default_headers({{"Access-Control-Allow-Origin", "*"},
                                {"Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS"},
                                {"Access-Control-Allow-Headers", "Content-Type"}});

    // 处理 OPTIONS 预检请求
    server.Options(".*", [](const httplib::Request &, httplib::Response &res)
                   { res.status = 204; });

    std::cout << "Accounting Server Started, Listening http://localhost:8080" << std::endl;

    server.listen("0.0.0.0", 8080);

    return 0;
}