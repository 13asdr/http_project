// 封转参数校验
#pragma once

#include "httplib.h"
#include "nlohmann/json.hpp"

#include "RecordDao.h"
#include "handler.h"
#include "Logger.h"

#include <iostream>

class Validator
{
public:
    using Request = httplib::Request;
    using Response = httplib::Response;
    using Json = nlohmann::json;


    static bool parsePositiveInt(const std::string& text, int &value);  //判断这个数是否是正整数    
    static bool validateLimit(const Request &req, Response &res, limit &l); //校验分页参数
    static bool validateRecordJson(const Json &j, Response &res);   //校验Record的JSON参数
    static bool validateUserJson(const Json &j, Response &res);  //校验User的JSON参数
};