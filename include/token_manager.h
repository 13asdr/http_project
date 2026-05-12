#pragma once

#include <chrono>
#include <string>

#include <jwt-cpp/jwt.h>
#include <jwt-cpp/traits/nlohmann-json/defaults.h>

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

    static std::string generate_token(size_t _user_id);
    static InfoToken validate_token(const std::string &_token);

private:
    static std::string secret_key;
};
