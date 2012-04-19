#ifndef EPYX_DHT_STORAGE_H
#define EPYX_DHT_STORAGE_H

#include <map>
#include <string>

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
            std::string get(const std::string& key);
            void set(const std::string& key, const std::string& data);

        private:
            std::map<std::string, std::string> data;
    };

}
}
#endif /*EPYX_DHT_STORAGE_H*/
