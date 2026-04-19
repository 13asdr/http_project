#include "token_manager.h"
// 初始化token容器
TokenManager::map_type TokenManager::tokens_ = {};
std::mutex TokenManager::mutex_;

std::string TokenManager::generate_random_string()
{
    static const std::string chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    std::random_device rd; // 每次重新构造，访问硬件
    std::uniform_int_distribution<int> dis(0, chars.size() - 1);

    std::string result(16, '\0');
    for (auto &c : result)
        c = chars[dis(rd)]; // rd 直接作为真随机引擎
    return result;
}

std::string TokenManager::generate_token(int _userId) // 生成token
{
    std::string token = generate_random_string();
    lock_guard lock(mutex_);
    tokens_.try_emplace(token, _userId);
    return token;
}
int TokenManager::validate_token(const std::string &_token) // 验证token
{
    lock_guard lock(mutex_);
    auto result = tokens_.find(_token);
    if (result == tokens_.end()) // 没找到token
        return -1;
    return result->second; // 找到了返回对应的userID
}
void TokenManager::remove_token(const std::string &_token) // 删除token
{
    lock_guard lock(mutex_);
    tokens_.erase(_token); // 移除token
}
