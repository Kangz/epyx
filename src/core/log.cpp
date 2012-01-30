#include "log.h"

namespace Epyx {
namespace log {

    Worker* _worker = NULL;
    TLSPointer<BufferContainer>* _buffers = NULL;
    EndlStruct endl;
    bool initialized = false;

    static BufferContainer* create_buffers(){
        return new BufferContainer;
    }

    void init(int flags, const std::string& file){
        EPYX_ASSERT_NO_LOG(Thread::isInitialized());
        _worker = new Worker(flags, file);
        _buffers = new TLSPointer<BufferContainer>(create_buffers);
        initialized = true;
    }

    Stream::Stream(int prio): priority(prio) {}

    Stream::~Stream(){}

    Stream& Stream::operator<<(const EndlStruct& f) {
        EPYX_ASSERT(log::initialized);
        std::ostringstream* buffer = &_buffers->get()->b[this->priority];
        _worker->write(buffer->str(), this->priority);
        buffer->str("");
        return *this;
    }

}
}
