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
    static void Add(RecordDao &_dao, const Request &_req, Response &_res);
    static void List(RecordDao &_dao, const Request &_req, Response &_res);
    static void StatByCategory(RecordDao &_dao, const Request &_req, Response &_res);
    static void Filter(RecordDao &_dao, const Request &_req, Response &_res); // 模糊查询备注关键词 AND 月份筛选
    static void Update(RecordDao &_dao, const Request &_req, Response &_res); // 更新函数
    static void Remove(RecordDao &_dao, const Request &_req, Response &_res); // 删除记录
    static void Export(RecordDao &_dao, const Request &_req, Response &_res); // 导出记录

    // user 相关接口
    static void Add(UserDao &_dao, const Request &_req, Response &_res);
    static void Update(UserDao &_dao, const Request &_req, Response &_res);
    static void Remove(UserDao &_dao, const Request &_req, Response &_res);

    // token 相关接口
    static void Login(UserDao &_dao, const Request &_req, Response &_res);
    static void Register(UserDao &_dao, const Request &_req, Response &_res);
    static void Logout(const Request &_req, Response &_res);

    // 统一的成功响应函数
    static void sendSuccess(Response &_res, const Json &_data, const std::string &_message);
    static void sendError(Response &_res, HttpStatus status, BusinessStatus code, const std::string &_message);

private:
    static int authCheck(const Request &_req, Response &_res); // 认证检查
    static void handleInternalError(const Request &_req, Response &_res, const std::exception &_e);
    static void JsonToRecord(Json &_j, Record &_r);
    static void RecordToJson(const Record &_r, Json &_j);
    static void JsonToUser(Json &_j, User &_u);
};

#endif
