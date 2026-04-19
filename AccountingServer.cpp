#include "httplib.h"
#include "nlohmann/json.hpp"
#include "record_dao.h"
#include "handler.h"
#include "config.h"
#include "logger.h"
#include "user_dao.h"

#include <filesystem>
#include <iostream>
#include <sstream>
#include <windows.h>

int main()
{
    SetConsoleOutputCP(65001); // 设置控制台输出为UTF-8编码,头文件<windows.h>

    std::filesystem::create_directory("logs");

    Logger::init();

    httplib::Server server; // 创建HTTP服务器

    Config config("config.ini"); // 解析配置文件
    DBconnect db(config.db);
    RecordDao dao(db);
    UserDao userDao(db);

    // POST
    // 添加记录 POST
    server.Post("/record/add", [&dao](const httplib::Request &req, httplib::Response &res)
                { Handler::add_record(dao, req, res); }); // 查询类别的总金额

    // 添加用户 POST
    server.Post("/user/login", [&userDao](const httplib::Request &req, httplib::Response &res)
                { Handler::login_user(userDao, req, res); }); // 登录
    server.Post("/user/register", [&userDao](const httplib::Request &req, httplib::Response &res)
                { Handler::register_user(userDao, req, res); }); // 注册
    server.Post("/user/logout", [&userDao](const httplib::Request &req, httplib::Response &res)
                { Handler::logout_user(req, res); }); // 退出登录

    // ==查询==
    server.Get("/record/statByCategory", [&dao](const httplib::Request &req, httplib::Response &res)
               { Handler::stat_by_category(dao, req, res); });
    // 查询所有记录 GET /record/list
    server.Get("/record/list", [&dao](const httplib::Request &req, httplib::Response &res)
               { Handler::list_records(dao, req, res); });

    // 模糊查询备注关键词 AND 月份筛选 GET /record/search?keyword=吃&month=2026-04
    server.Get("/record/filter", [&dao](const httplib::Request &req, httplib::Response &res)
               { Handler::filter_records(dao, req, res); });

    // ==更新==
    // 更新函数
    server.Put("/record/update", [&dao](const httplib::Request &req, httplib::Response &res)
               { Handler::update_record(dao, req, res); });

    // ==删除==
    // 删除记录 DELETE /record/delete?id=1
    server.Delete("/record/delete", [&dao](const httplib::Request &req, httplib::Response &res)
                  { Handler::remove_record(dao, req, res); });

    // ==导出==
    // 导出记录 GET /record/export
    server.Get("/record/export", [&dao](const httplib::Request &req, httplib::Response &res)
               { Handler::export_records(dao, req, res); });

    // 允许所有跨域请求
    server.set_default_headers({{"Access-Control-Allow-Origin", "*"},
                                {"Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS"},
                                {"Access-Control-Allow-Headers", "Content-Type, token"}});
    // 处理 OPTIONS 预检请求
    server.Options(".*", [](const httplib::Request &, httplib::Response &res)
                   { res.status = 204; });

    std::ostringstream information;
    information << "Accounting Server Started, Listening http://" << config.server.host << ":" << config.server.port;

    Logger::info(information.str());
    bool ok = server.listen(config.server.host.c_str(), config.server.port); // ctrl+c stop
    Logger::info("listen return: " + std::to_string(ok));

    return 0;
}