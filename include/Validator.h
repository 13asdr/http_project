// 封转参数校验
#pragma once

#include "httplib.h"
#include "nlohmann/json.hpp"

#include "RecordDao.h"
#include "Status.h"

#include <iostream>

struct ValidationResult
{
    bool is_valid = true;
    message_code code = message_code::Success;
    std::string message;
};

class Validator
{
public:
    using Request = httplib::Request;
    using Json = nlohmann::json;

    static ValidationResult parsePositiveInt(const std::string &text, int &value);
    static ValidationResult validateLimit(const Request &req, limit &l);
    static ValidationResult validateRecordJson(const Json &j);
    static ValidationResult validateUserJson(const Json &j);

private:
    static ValidationResult buildResult(bool is_valid, message_code code, const std::string &message)
    {
        ValidationResult result;
        result.is_valid = is_valid;
        result.code = code;
        result.message = message;
        return result;
    }
};
