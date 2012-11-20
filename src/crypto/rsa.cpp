#include "rsa.h"
#include "sha2.h"
#include <cryptopp/rsa.h>
#include <cryptopp/filters.h>
#include <cryptopp/osrng.h>
#include <cryptopp/pssr.h>
#include "../core/common.h"

namespace Epyx
{
namespace crypto
{
    PublicKey::PublicKey(CryptoPP::RSA::PublicKey p) {
        this->pub = p;
    }

    std::string PublicKey::dump() const {
        std::string digest;
        CryptoPP::StringSink sink(digest);
        this->pub.Save(sink);

        return digest;
    }

    std::string PublicKey::encrypt(std::string plain) {
        CryptoPP::AutoSeededRandomPool rnd;
        CryptoPP::RSAES_OAEP_SHA_Encryptor e(this->pub);
        std::string cipher;
        CryptoPP::StringSource(plain, true,
                new CryptoPP::PK_EncryptorFilter(rnd, e,
                    new CryptoPP::StringSink(cipher)));
        return cipher;
    }

    bool PublicKey::verify(std::string plain, std::string signature) {
        CryptoPP::RSASS<CryptoPP::PSS, CryptoPP::SHA1>::Verifier v(this->pub);
        try {
            CryptoPP::StringSource(plain+signature, true,
                new CryptoPP::SignatureVerificationFilter(v, NULL, CryptoPP::SignatureVerificationFilter::THROW_EXCEPTION));
        }
        catch(CryptoPP::Exception e) {
            return false;
        }
        return true;
    }

    PrivateKey::PrivateKey(CryptoPP::RSA::PrivateKey k) {
        this->key = k;
    }
    PrivateKey* PrivateKey::generate() {
        bool again;
        CryptoPP::AutoSeededRandomPool rnd;
        CryptoPP::RSA::PrivateKey rsaPrivate;
        do {
            again = false;
            log::debug<<"Generating a key"<<log::endl;
            rsaPrivate.GenerateRandomWithKeySize(rnd, 3072);
            if(! rsaPrivate.Validate(rnd, 1)) {
               again = true;
               log::debug<<"Bad key!"<<log::endl;
            }
        } while(again);

       PrivateKey* priv = new PrivateKey(rsaPrivate);
       return priv;
    }

    std::string PrivateKey::dump() const {
        std::string digest;
        CryptoPP::StringSink sink(digest);
        this->key.Save(sink);

        return digest;
    }

    PublicKey PrivateKey::getPublicKey() {
        PublicKey pub(this->key);
        return pub;
    }

    std::string PrivateKey::decrypt(std::string cipher) {
        CryptoPP::AutoSeededRandomPool rnd;
        CryptoPP::RSAES_OAEP_SHA_Decryptor d(this->key);
        std::string plain;
        CryptoPP::StringSource(cipher, true,
                new CryptoPP::PK_DecryptorFilter(rnd, d,
                    new CryptoPP::StringSink(plain)));
        return plain;
    }

    std::string PrivateKey::sign(std::string plain) {
        CryptoPP::RSASS<CryptoPP::PSS, CryptoPP::SHA1>::Signer s(this->key);
        std::string sign;
        CryptoPP::AutoSeededRandomPool rnd;
        CryptoPP::StringSource(plain, true,
                new CryptoPP::SignerFilter(rnd, s,
                    new CryptoPP::StringSink(sign)));
        return sign;
    }
 
    std::ostream& operator<<(std::ostream& os, const PublicKey& pub) {
        os << Base64(pub.dump());
        return os;
    }
    std::ostream& operator<<(std::ostream& os, const PrivateKey& key) {
        os << Base64(key.dump());
        return os;
    }
}
}
