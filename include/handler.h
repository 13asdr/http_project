#ifndef HANDLER_H
#define HANDLER_H

#include "httplib.h"
#include "nlohmann/json.hpp"
#include "record_dao.h"
#include "user_dao.h"
#include "token_manager.h"
#include "validator.h"
#include "logger.h"
#include "status.h"

#include <iostream>

class Handler
{
public:
    using Request = httplib::Request;
    using Response = httplib::Response;
    using Json = nlohmann::json;

    // record 相关接口
    static void Add(RecordDao &dao, const Request &req, Response &res);
    static void List(RecordDao &dao, const Request &req, Response &res);
    static void StatByCategory(RecordDao &dao, const Request &req, Response &res);
    static void Filter(RecordDao &dao, const Request &req, Response &res); // 模糊查询备注关键词 AND 月份筛选
    static void Update(RecordDao &dao, const Request &req, Response &res); // 更新函数
    static void Remove(RecordDao &dao, const Request &req, Response &res); // 删除记录
    static void Export(RecordDao &dao, const Request &req, Response &res); // 导出记录

    // user 相关接口
    static void Add(UserDao &dao, const Request &req, Response &res);
    static void Update(UserDao &dao, const Request &req, Response &res);
    static void Remove(UserDao &dao, const Request &req, Response &res);

    // token 相关接口
    static void Login(UserDao &dao, const Request &req, Response &res);
    static void Register(UserDao &dao, const Request &req, Response &res);
    static void Logout(const Request &req, Response &res);

    // 统一的成功响应函数
    static void sendSuccess(Response &res, const Json &data, const std::string &message);
    static void sendError(Response &res, HttpStatus status, BusinessStatus code, const std::string &message);

private:
    static int authCheck(const Request &req, Response &res); // 认证检查
    static void handleInternalError(const Request &req, Response &res, const std::exception &e);
    static void JsonToRecord(Json &j, Record &r);
    static void RecordToJson(const Record &r, Json &j);
    static void JsonToUser(Json &j, User &u);
};

#endif
