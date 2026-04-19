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
    static void add_record(RecordDao &_dao, const Request &_req, Response &_res);
    static void list_records(RecordDao &_dao, const Request &_req, Response &_res);
    static void stat_by_category(RecordDao &_dao, const Request &_req, Response &_res);
    static void filter_records(RecordDao &_dao, const Request &_req, Response &_res); // 模糊查询备注关键词 AND 月份筛选
    static void update_record(RecordDao &_dao, const Request &_req, Response &_res); // 更新函数
    static void remove_record(RecordDao &_dao, const Request &_req, Response &_res); // 删除记录
    static void export_records(RecordDao &_dao, const Request &_req, Response &_res); // 导出记录

    // user 相关接口
    static void add_user(UserDao &_dao, const Request &_req, Response &_res);
    static void update_user(UserDao &_dao, const Request &_req, Response &_res);
    static void remove_user(UserDao &_dao, const Request &_req, Response &_res);

    // token 相关接口
    static void login_user(UserDao &_dao, const Request &_req, Response &_res);
    static void register_user(UserDao &_dao, const Request &_req, Response &_res);
    static void logout_user(const Request &_req, Response &_res);

    // 统一的成功响应函数
    static void send_success(Response &_res, const Json &_data, const std::string &_message);
    static void send_error(Response &_res, HttpStatus status, BusinessStatus code, const std::string &_message);

private:
    static int auth_check(const Request &_req, Response &_res); // 认证检查
    static void handle_internal_error(const Request &_req, Response &_res, const std::exception &_e);
    static void json_to_record(Json &_j, Record &_r);
    static void record_to_json(const Record &_r, Json &_j);
    static void json_to_user(Json &_j, User &_u);
};

#endif
