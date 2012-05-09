#include <string>
#include "core/common.h"
#include "crypto/sha2.h"

using namespace Epyx;

void test_sha2(){
    std::string message = "Cool, this is Epyx";
    std::string digest = crypto::Base64(crypto::SHA256(message));
    log::info<<"SHA256|Base64Encoding \"" << message << "\":" << digest << log::endl;
}

int main(){
    Thread::init();
    log::init(log::CONSOLE);

    test_sha2();

    log::flushAndQuit();
    return 0;
}
