#include "log.h"

namespace Epyx {
namespace log {

    Worker* _worker = NULL;
    TLSPointer<BufferContainer>* _buffers = NULL;
    EndlStruct endl;


    static BufferContainer* create_buffers(){
        return new BufferContainer;
    }

    void init(int flags, const std::string& file){
        _worker = new Worker(flags, file);
        _buffers = new TLSPointer<BufferContainer>(create_buffers);
    }

    Stream::Stream(int prio): priority(prio) {}

    Stream::~Stream(){}

    Stream& Stream::operator<<(const EndlStruct& f) {
        std::ostringstream* buffer = &_buffers->get()->b[this->priority];
        _worker->write(buffer->str(), this->priority);
        buffer->str("");
        return *this;
    }

}
}
