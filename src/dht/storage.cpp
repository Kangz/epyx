#include "storage.h"

namespace Epyx
{
namespace DHT
{

    Storage::Storage(){}

    bool Storage::has(const std::string& key){
        std::lock_guard<std::mutex> guard(lock);
        bool result = data.count(key) != 0;
        return result;
    }

    bool Storage::get(const std::string& key, Value& res){
        std::lock_guard<std::mutex> guard(lock);
        bool result = false;
        std::map<std::string, Value>::iterator it = data.find(key);
        if (it != data.end()) {
            res = (*it).second;
            result = true;
        }
        return result;
    }

    void Storage::set(const std::string& key, const std::string& datum){
        std::lock_guard<std::mutex> guard(lock);
        Value v(datum);
        if(data.find(key) == data.end()) {
            data.insert(std::make_pair(key, datum));
        } else {
            (*data.find(key)).second = datum;
        }
    }

}
}
