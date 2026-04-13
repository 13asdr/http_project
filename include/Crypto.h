// 哈希加密类   crypto 加密
#pragma once
#include <string>
#include <windows.h>
#include <wincrypt.h>
#include <sstream>
#include <iomanip>

class Crypto
{
public:
    // SHA256 加密
    static std::string sha256(const std::string &input)
    {
        HCRYPTPROV hProv = 0;
        HCRYPTHASH hHash = 0;
        BYTE hash[32];
        DWORD hashLen = 32;

        CryptAcquireContext(&hProv, nullptr, nullptr, PROV_RSA_AES, CRYPT_VERIFYCONTEXT);
        CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash);
        CryptHashData(hHash, (BYTE *)input.c_str(), input.size(), 0);
        CryptGetHashParam(hHash, HP_HASHVAL, hash, &hashLen, 0);

        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);

        std::ostringstream oss;
        for (int i = 0; i < 32; i++)
            oss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        return oss.str();
    }
};
