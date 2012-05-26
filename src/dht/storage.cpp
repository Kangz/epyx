#include "storage.h"

namespace Epyx
{
namespace DHT
{

    Storage::Storage(){};

    bool Storage::has(const std::string& key){
        lock.lock();
        bool result = data.count(key) != 0;
        lock.unlock();
        return result;
    }

    bool Storage::get(const std::string& key, Value& res){
        bool result = false;
        lock.lock();
        std::map<std::string, Value>::iterator it = data.find(key);
        if (it != data.end()) {
            res = (*it).second;
        }
        result = true;
        lock.unlock();
        return result;
    }

    void Storage::set(const std::string& key, const std::string& datum){
        lock.lock();
        Value v(datum);
        data.insert(std::make_pair(key, datum));
        lock.unlock();
    }

}
}
