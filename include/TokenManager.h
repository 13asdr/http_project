#pragma once
#include <map>
#include <string>
#include <iostream>
#include <mutex>
#include <random>

class TokenManager
{
public:
    using map_type = std::map<std::string, int>;        //通过token查找userId
    using lock_guard = std::lock_guard<std::mutex>;

    static std::string generate_token(int userId);     // 生成token
    static int validate_token(const std::string &token);     // 验证token
    static void remove_token(const std::string &token);     // 删除token


private:
    static std::string generate_random_string();     // 生成随机字符串

    static map_type tokens_;
    static std::mutex mutex_;
};
