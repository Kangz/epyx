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
