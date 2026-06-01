#include "token_manager.h"

std::string TokenManager::secret_key = JwtConfig("jwt.ini").get_secret_key();

std::string TokenManager::generate_token(size_t _user_id)
{
    auto token = jwt::create<jwt_traits>()
                     .set_algorithm("HS256")
                     .set_type("JWT")
                     .set_issuer("AccountingServer")
                     .set_subject(std::to_string(_user_id))
                     .set_issued_at(std::chrono::system_clock::now())
                     .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours(24))
                     .sign(jwt::algorithm::hs256{TokenManager::secret_key});

    Logger::info("Generated token for user_id " + std::to_string(_user_id));
    return token;
}

InfoToken TokenManager::validate_token(const std::string &_token)
{
    if (_token.empty())
    {
        Logger::error("Token validation failed: token is empty");
        return InfoToken{0, JwtStatus::Invalid};
    }

    try
    {
        auto decoded = jwt::decode<jwt_traits>(_token);

        auto verifier = jwt::verify<jwt_traits>()
                            .allow_algorithm(jwt::algorithm::hs256{TokenManager::secret_key})
                            .with_issuer("AccountingServer");

        verifier.verify(decoded);

        size_t user_id = std::stoull(decoded.get_subject());
        Logger::info("Token validation succeeded, user_id: " + std::to_string(user_id));

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
        Logger::error("Token validation error: " + std::string(e.what()));
        return InfoToken{0, JwtStatus::Invalid};
    }
}
