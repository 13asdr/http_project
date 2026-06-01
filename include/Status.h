#pragma once

enum class MessageCode
{
    Success = 0,
    InvalidJson = 4000,
    InvalidParam = 4001,
    Unauthorized = 4010,
    TokenExpired = 4011,
    InternalError = 5000
};

enum class HttpStatus
{
    Success = 200,
    BadRequest = 400,
    Unauthorized = 401,
    InternalError = 500
};

enum class JwtStatus
{
    Valid = 0,   
    Invalid = 1,
    Expired = 2
};

inline MessageCode jwt_status_to_message_code(JwtStatus _jwt_status)
{
    switch (_jwt_status)
    {
    case JwtStatus::Valid:
        return MessageCode::Success;
    case JwtStatus::Invalid:
        return MessageCode::Unauthorized;
    case JwtStatus::Expired:
        return MessageCode::TokenExpired;
    default:
        return MessageCode::InternalError;
    }
}

inline const char *jwt_status_to_message(JwtStatus _jwt_status)
{
    switch (_jwt_status)
    {
    case JwtStatus::Valid:
        return "success";
    case JwtStatus::Invalid:
        return "invalid token";
    case JwtStatus::Expired:
        return "token expired";
    default:
        return "internal server error";
    }
}
