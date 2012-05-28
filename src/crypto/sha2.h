#ifndef EPYX_CRYPTO_SHA2_H
#define EPYX_CRYPTO_SHA2_H

#include "string"
#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include <cryptopp/base64.h>

namespace Epyx
{
namespace crypto
{
    std::string SHA256(const std::string& message);
    std::string Base64(const std::string& message);
}
}


#endif //EPYX_CRYPTO_SHA2_H
