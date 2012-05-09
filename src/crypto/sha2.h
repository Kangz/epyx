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

    //CryptoPP has a strange way to chain filters, there is no memory leak here.
    std::string SHA256(const std::string& message){
        std::string digest;
        CryptoPP::SHA256 hash;
        CryptoPP::StringSource enc(message, true,
            new CryptoPP::HashFilter(hash,
                new CryptoPP::StringSink(digest)
            )
        );

        return digest;
    }

    std::string Base64(const std::string& message){
        std::string digest;
        CryptoPP::StringSource enc(message, true,
            new CryptoPP::Base64Encoder(
                new CryptoPP::StringSink(digest)
            )
        );

        return digest;
    }

}
}


#endif //EPYX_CRYPTO_SHA2_H
