#pragma once

enum class MessageCode
{
    Success = 0,
    InvalidJson = 4000,
    InvalidParam = 4001,
    Unauthorized = 4010,
    InternalError = 5000
};

enum class HttpStatus
{
    Success = 200,
    BadRequest = 400,
    Unauthorized = 401,
    InternalError = 500
};
