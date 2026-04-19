#pragma once

#include "httplib.h"
#include "nlohmann/json.hpp"

#include "record_dao.h"
#include "status.h"

#include <string>

struct ValidationResult
{
    bool is_valid = true;
    MessageCode code = MessageCode::Success;
    std::string message;
};

class Validator
{
public:
    using Request = httplib::Request;
    using Json = nlohmann::json;

    static ValidationResult parse_positive_int(const std::string &_text, int &_value);
    static ValidationResult validate_limit(const Request &_req, Limit &_limit);
    static ValidationResult validate_record_json(const Json &_json);
    static ValidationResult validate_user_json(const Json &_json);

private:
    static ValidationResult build_result(bool _is_valid, MessageCode _code, const std::string &_message)
    {
        ValidationResult result;
        result.is_valid = _is_valid;
        result.code = _code;
        result.message = _message;
        return result;
    }
};
