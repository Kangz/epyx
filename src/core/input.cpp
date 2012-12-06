#include "input.h"
#include <csignal>
#include <mutex>
#include "log.h"
namespace Epyx
{
    bool Input::waiting = false;
    std::condition_variable Input::waitingCondition;

    void Input::waitForInt() {
        waiting = true;
        void (*oldSigHandler)(int);
        oldSigHandler = signal(SIGINT, Input::sighandler);
        while (waiting) {
            std::mutex m;
            std::unique_lock<std::mutex> lock(m);
            waitingCondition.wait(lock);
        }
        signal(SIGINT, oldSigHandler);
    }

    void Input::sighandler(int signal) {
        waiting = false;
        waitingCondition.notify_all();
    }
}
