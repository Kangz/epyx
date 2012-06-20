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
