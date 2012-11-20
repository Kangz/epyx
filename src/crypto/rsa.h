/*
 *   Copyright 2012 Epyx Team
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */
#ifndef EPYX_CRYPTO_RSA_H
#define EPYX_CRYPTO_RSA_H

#include "string"
#include <cryptopp/rsa.h>
#include <cryptopp/filters.h>

namespace Epyx
{
namespace crypto
{
    class PublicKey {
        private:
            CryptoPP::RSA::PublicKey pub;

        public:
            PublicKey(CryptoPP::RSA::PublicKey);
            std::string dump() const;
            std::string encrypt(std::string message);
            bool verify(std::string plain, std::string signature);
    };

    class PrivateKey {
        private:
            CryptoPP::RSA::PrivateKey key;

        public:
            PrivateKey(CryptoPP::RSA::PrivateKey k);
            static PrivateKey* generate();
            std::string dump() const;
            PublicKey getPublicKey();
            std::string decrypt(std::string cipher);
            std::string sign(std::string plain);
    };

    std::ostream& operator<<(std::ostream& os, const PublicKey& pub);
    std::ostream& operator<<(std::ostream& os, const PrivateKey& key);
}
}


#endif //EPYX_CRYPTO_RSA_H
