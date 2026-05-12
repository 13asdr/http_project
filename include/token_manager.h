#pragma once
#include <iostream>
#include <string>
#include <jwt-cpp/jwt.h>
#include <jwt-cpp/traits/nlohmann-json/defaults.h> // ← 用这个
#include <chrono>

#include "config.h"
#include "logger.h"
#include "status.h"

struct InfoToken
{    
    size_t user_id;
    JwtStatus status;
};

class TokenManager
{
public:
    using jwt_traits = jwt::traits::nlohmann_json;

    static std::string generate_token(size_t _userid);       // 生成token
    static InfoToken validate_token(const std::string &_token); // 验证token

private:
    static std::string secret_key; // 从配置文件获取 secret key
};
