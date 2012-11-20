#include "storage.h"

namespace Epyx
{
namespace DHT
{

    Storage::Storage(ActorId<ReplicaterActor> replicater): replicater(replicater) {
    }

    bool Storage::has(const std::string& key){
        std::lock_guard<std::mutex> guard(lock);
        bool result = data.count(key) != 0;
        return result;
    }

    bool Storage::get(const std::string& key, Value& res){
        std::lock_guard<std::mutex> guard(lock);
        std::map<std::string, Value>::iterator it = data.find(key);
        if (it != data.end()) {
            res = (*it).second;
            return true;
        }
        return false;
    }

    void Storage::set(const std::string& key, const std::string& datum){
        bool newKey = false;
        {
            std::lock_guard<std::mutex> guard(lock);
            Value v(datum);
            if(data.find(key) == data.end()) {
                newKey = true;
                data.insert(std::make_pair(key, datum));
            } else {
                (*data.find(key)).second = datum;
            }
        }

        if(newKey){
            replicater.post(EPYX_AQ("new key"), key);
        }
    }

}
}
