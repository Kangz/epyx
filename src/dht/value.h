/**
 * @file dht/value.h
 */
#ifndef EPYX_DHT_VALUE_H
#define EPYX_DHT_VALUE_H

#include <string>

namespace Epyx
{
namespace DHT
{

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
