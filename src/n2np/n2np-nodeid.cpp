#include "n2np-nodeid.h"
#include "../core/common.h"

namespace Epyx
{

    N2npNodeId::N2npNodeId()
    :name(), relay() {
    }

    N2npNodeId::N2npNodeId(const Address& relay)
    :name(), relay(relay) {
        this->name[0] = 0;
    }

    N2npNodeId::N2npNodeId(const std::string& name, const Address& relay)
    :name(name), relay(relay) {
    }

    N2npNodeId::N2npNodeId(const std::string& str) {
        size_t pos = str.find('@');
        if (pos == std::string::npos) {
            log::error << "N2NP: Arobas is missing in " << str << log::endl;
            throw FailException("N2np::NodeId", "No @ in ID");
        }
        name = str.substr(0, pos);
        relay = Address(str.substr(pos+1));
    }

    N2npNodeId::N2npNodeId(const N2npNodeId& nodeid)
    :name(nodeid.name), relay(nodeid.relay) {
    }

    N2npNodeId& N2npNodeId::operator=(const N2npNodeId& nodeid) {
        if (this != &nodeid) {
            this->name = nodeid.name;
            this->relay = nodeid.relay;
        }
        return *this;
    }

    std::ostream& operator<<(std::ostream& os, const N2npNodeId& nodeid) {
        return os << nodeid.name << '@' << nodeid.relay;
    }

    std::string N2npNodeId::toString() const {
        std::ostringstream stream;
        stream << *this;
        return stream.str();
    }

    bool operator==(const N2npNodeId& nodeid1, const N2npNodeId& nodeid2) {
        return (!nodeid1.name.compare(nodeid2.name))
            && (nodeid1.relay == nodeid2.relay);
    }

    bool operator!=(const N2npNodeId& nodeid1, const N2npNodeId& nodeid2) {
        return !(nodeid1 == nodeid2);
    }

    const std::string& N2npNodeId::getName() const {
        return name;
    }

    const Address& N2npNodeId::getRelay() const {
        return relay;
    }
}
