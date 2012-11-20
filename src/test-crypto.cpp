#include <string>
#include "core/common.h"
#include "crypto/sha2.h"
#include "crypto/rsa.h"

using namespace Epyx;

void test_sha2(){
    std::string message = "Cool, this is Epyx";
    std::string digest = crypto::Base64(crypto::SHA256(message));
    log::info<<"SHA256|Base64Encoding \"" << message << "\":" << digest << log::endl;
}

void test_rsa(){
    crypto::PrivateKey* key = crypto::PrivateKey::generate();
    log::info<<"New RSA key generated"<<log::endl;
    log::info<<"Private dump:\n"<<key<<log::endl;
    log::info<<"Public dump:\n"<<key->getPublicKey()<<log::endl;
    std::string m("Epyx is cool!");
    std::string e = key->getPublicKey().encrypt(m);
    log::info<<"Encrypted: \n"<<crypto::Base64(e)<<log::endl;
    std::string r = key->decrypt(e);
    log::info<<"Decrypted: "<<r<<log::endl;
    log::info<<"Decrypted (B64): \n"<<crypto::Base64(r)<<log::endl;
    std::string s = key->sign(m);
    log::info<<"Signed message with signature:\n"<<crypto::Base64(s)<<log::endl;
    std::string result = (key->getPublicKey().verify(m, s))? "OK.":"Failed !!!";
    log::info<<"Verification: "<<result<<log::endl;
    delete key;
}

int main(){
    Thread::init();
    log::init(log::CONSOLE);

    test_rsa();

    log::flushAndQuit();
    return 0;
}
