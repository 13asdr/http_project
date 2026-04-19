#pragma once

enum class BusinessStatus // businessStatusCode
{
    Success = 0,
    InvalidJSON = 4000,   // 请求JSON格式错误
    InvalidPARAM = 4001,  // 请求参数有问题，比如分页参数不合法，或者记录的金额为负数等
    Unauthorized = 4010,  // 没登录，或 token 无效
    InternalError = 5000, // 服务器内部错误
    Exception = 5001      // 异常错误
};

enum class HttpStatus // HTTP状态码
{
    Success = 200,
    BadRequest = 400,    // 请求格式或参数有问题
    Unauthorized = 401,   // 没登录，或 token 无效
    InternalError = 500, // 服务器内部错误
};