#include "token_manager.h"

std::string TokenManager::secret_key = Config_Jwt("jwt.ini").get_secret_key();

std::string TokenManager::generate_token(size_t _userid) // 生成token
{
    auto token = jwt::create<jwt_traits>()
                     .set_algorithm("HS256")
                     .set_type("JWT")
                     .set_issuer("AccountingServer")
                     .set_subject(std::to_string(_userid))
                     .set_issued_at(std::chrono::system_clock::now())
                     .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours(24))
                     .set_payload_claim("sample", jwt::claim(std::string("test")))
                     .sign(jwt::algorithm::hs256{TokenManager::secret_key});

    Logger::info("Generated token for user_id " + std::to_string(_userid) + ": " + token);

    return token;
}
InfoToken TokenManager::validate_token(const std::string &_token) // 验证token
{
    try
    {
        auto decoded = jwt::decode<jwt_traits>(_token);

        auto verifier = jwt::verify<jwt_traits>()
                            .allow_algorithm(jwt::algorithm::hs256{TokenManager::secret_key}) // 指定算法和密钥
                            .with_issuer("AccountingServer")                          // 验证 iss
                            .with_claim("sample", jwt::claim(std::string("test")));
        // exp 过期时间自动验证，不需要手动写

        verifier.verify(decoded); // 失败抛异常
        // 验证通过，读取数据
        size_t user_id = std::stoul(decoded.get_subject());
        std::string sample = decoded.get_payload_claim("sample").as_string();

        Logger::info("user_id: " + std::to_string(user_id) + ", sample: " + sample);

        return InfoToken{user_id, JwtStatus::Valid};
    }
    catch (const jwt::error::token_verification_exception &e)
    {
        auto code = e.code();

        if (code == jwt::error::token_verification_error::token_expired)
        {
            Logger::error("Token expired: " + std::string(e.what()));
            return InfoToken{0, JwtStatus::Expired};
        }
        Logger::error("Token verification failed: " + std::string(e.what()));
        return InfoToken{0, JwtStatus::Invalid};
    }
    catch (const std::exception &e)
    {
        Logger::error("error: " + std::string(e.what()));
        return InfoToken{0, JwtStatus::Invalid};
    }
}
