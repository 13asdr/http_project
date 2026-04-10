#include "httplib.h"
#include "nlohmann/json.hpp"
#include "RecordDao.h"
#include "handler.h"
#include "Config.h"
#include "Logger.h"
#include "UserDao.h"

#include <filesystem>
#include <iostream>
#include <sstream>
#include <windows.h>

int main()
{
    SetConsoleOutputCP(65001); // 设置控制台输出为UTF-8编码

    std::filesystem::create_directory("logs");

    Logger::init();

    httplib::Server server; // 创建HTTP服务器

    Config config("config.ini"); // 解析配置文件
    DBconnect db(config.db);
    RecordDao dao(db);
    UserDao userDao(db);

    // ==添加==
    // 添加记录 POST /record/add
    server.Post("/record/add", [&dao](const httplib::Request &req, httplib::Response &res)
                { Handler::Add(dao, req, res); }); // 查询类别的总金额

    server.Post("/user/add", [&dao](const httplib::Request &req, httplib::Response &res)
                { Handler::Add(dao, req, res); }); // 添加用户

    // ==查询==
    server.Get("/record/statByCategory", [&dao](const httplib::Request &req, httplib::Response &res)
               { Handler::StatByCategory(dao, req, res); });
    // 查询所有记录 GET /record/list
    server.Get("/record/list", [&dao](const httplib::Request &req, httplib::Response &res)
               { Handler::List(dao, req, res); });
    // 按月份查询 GET /record/listByMonth?month=2026-04
    server.Get("/record/listByMonth", [&dao](const httplib::Request &req, httplib::Response &res)
               { Handler::ListByMonth(dao, req, res); });
    // 模糊查询备注关键词 GET /record/search?keyword=吃
    server.Get("/record/search", [&dao](const httplib::Request &req, httplib::Response &res)
               { Handler::Search(dao, req, res); });
    // 模糊查询备注关键词 AND 月份筛选 GET /record/search?keyword=吃&month=2026-04
    server.Get("/record/filter", [&dao](const httplib::Request &req, httplib::Response &res)
               { Handler::Filter(dao, req, res); });

    // ==更新==
    // 更新函数
    server.Put("/record/update", [&dao](const httplib::Request &req, httplib::Response &res)
               { Handler::Update(dao, req, res); });

    // ==删除==
    // 删除记录 DELETE /record/delete?id=1
    server.Delete("/record/delete", [&dao](const httplib::Request &req, httplib::Response &res)
                  { Handler::Remove(dao, req, res); });

    // ==导出==
    // 导出记录 GET /record/export
    server.Get("/record/export", [&dao](const httplib::Request &req, httplib::Response &res)
               { Handler::Export(dao, req, res); });

    // 允许所有跨域请求
    server.set_default_headers({{"Access-Control-Allow-Origin", "*"},
                                {"Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS"},
                                {"Access-Control-Allow-Headers", "Content-Type"}});
    // 处理 OPTIONS 预检请求
    server.Options(".*", [](const httplib::Request &, httplib::Response &res)
                   { res.status = 204; });

    std::ostringstream information;
    information << "Accounting Server Started, Listening http://" << config.server.host << ":" << config.server.port;

    Logger::info(information.str());
    server.listen(config.server.host, config.server.port); // ctrl+c stop

    return 0;
}