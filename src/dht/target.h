/**
 * @file dht/target.h
 */
#ifndef EPYX_DHT_TARGET_H
#define EPYX_DHT_TARGET_H

#include "../n2np/node.h"
#include "id.h"

namespace Epyx
{
namespace DHT
{

    struct Target {
        N2NP::Node& myN2np;
        N2NP::NodeId n2npId;
        Id id;
        Target(N2NP::Node& n, N2NP::NodeId& sN2np, Id& s)
        :myN2np(n), n2npId(sN2np), id(s) {
        }
    };

}
}

#endif //EPYX_DHT_TARGET_H
