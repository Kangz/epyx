#include "nodeid.h"
#include "../core/common.h"

namespace Epyx
{
    namespace N2NP
    {

        NodeId::NodeId()
        :name(), relay() {
        }

        NodeId::NodeId(const Address& relay)
        :name(), relay(relay) {
            this->name[0] = 0;
        }

        NodeId::NodeId(const std::string& name, const Address& relay)
        :name(name), relay(relay) {
        }

        NodeId::NodeId(const std::string& str) {
            size_t pos = str.find('@');
            if (pos == std::string::npos) {
                log::error << "N2NP: Arobas is missing in " << str << log::endl;
                throw ParserException("N2np::NodeId", "No @ in ID");
            }
            name = str.substr(0, pos);
            relay = Address(str.substr(pos + 1));
        }

        NodeId::NodeId(const NodeId& nodeid)
        :name(nodeid.name), relay(nodeid.relay) {
        }

        NodeId& NodeId::operator=(const NodeId& nodeid) {
            if (this != &nodeid) {
                this->name = nodeid.name;
                this->relay = nodeid.relay;
            }
            return *this;
        }

        std::ostream& operator<<(std::ostream& os, const NodeId& nodeid) {
            return os << nodeid.name << '@' << nodeid.relay;
        }

        std::string NodeId::toString() const {
            std::ostringstream stream;
            stream << *this;
            return stream.str();
        }

        int NodeId::compare(const NodeId& nodeid) const {
            int c = relay.compare(nodeid.relay);
            if (c != 0) return c;
            return name.compare(nodeid.name);
        }

        bool operator==(const NodeId& nodeid1, const NodeId& nodeid2) {
            return nodeid1.compare(nodeid2) == 0;
        }

        bool operator!=(const NodeId& nodeid1, const NodeId& nodeid2) {
            return nodeid1.compare(nodeid2) != 0;
        }

        bool operator<(const NodeId& nodeid1, const NodeId& nodeid2) {
            return nodeid1.compare(nodeid2) < 0;
        }

        const std::string& NodeId::getName() const {

            return name;
        }

        const Address& NodeId::getRelay() const {
            return relay;
        }
    }
}
