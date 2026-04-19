// 封转参数校验
#pragma once

#include "httplib.h"
#include "nlohmann/json.hpp"

#include "record_dao.h"
#include "status.h"

#include <iostream>

struct ValidationResult
{
    bool is_valid = true;
    BusinessStatus code = BusinessStatus::Success;
    std::string message;
};

class Validator
{
public:
    using Request = httplib::Request;
    using Json = nlohmann::json;

    static ValidationResult parsePositiveInt(const std::string &_text, int &_value);
    static ValidationResult validateLimit(const Request &_req, Limit &_l);
    static ValidationResult validateRecordJson(const Json &_j);
    static ValidationResult validateUserJson(const Json &_j);

private:
    static ValidationResult buildResult(bool _is_valid, BusinessStatus _code, const std::string &_message)
    {
        ValidationResult result;
        result.is_valid = _is_valid;
        result.code = _code;
        result.message = _message;
        return result;
    }
};
