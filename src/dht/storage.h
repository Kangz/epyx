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
 * @file dht/storage.h
 */
#ifndef EPYX_DHT_STORAGE_H
#define EPYX_DHT_STORAGE_H

#include <map>
#include <string>
#include "../core/common.h"
#include "value.h"

namespace Epyx
{
namespace DHT
{

    //For now it is a proxy class but we will need it for the replicator
    class Storage
    {
        public:
            Storage();
            bool has(const std::string& key);
            bool get(const std::string& key, Value& res);
            void set(const std::string& key, const std::string& data);

        private:
            Mutex lock;
            std::map<std::string, Value> data;
    };

}
}
#endif /*EPYX_DHT_STORAGE_H*/
