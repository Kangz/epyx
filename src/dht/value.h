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
 * @file dht/value.h
 * @brief containers for values in the Storage
 */
#ifndef EPYX_DHT_VALUE_H
#define EPYX_DHT_VALUE_H

#include <string>

namespace Epyx
{
namespace DHT
{

    /**
     * @class Value
     */
    struct Value {
        std::string content;
        Value(const std::string& c)
        :content(c){
        }
        Value(){
        }
    };

}
}
#endif /*EPYX_DHT_VALUE_H*/
