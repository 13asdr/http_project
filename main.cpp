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
        result.push_back(item);
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
                                {"Access-Control-Allow-Methods", "GET, POST, DELETE, OPTIONS"},
                                {"Access-Control-Allow-Headers", "Content-Type"}});

    // 处理 OPTIONS 预检请求
    server.Options(".*", [](const httplib::Request &, httplib::Response &res)
                   { res.status = 204; });

    std::cout << "Accounting Server Started, Listening http://localhost:8080" << std::endl;

    server.listen("0.0.0.0", 8080);

    return 0;
}