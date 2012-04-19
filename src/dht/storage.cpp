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

    std::string Storage::get(const std::string& key){
        lock.lock();
        std::string& result = data[key];
        lock.unlock();
        return result;
    }

    void Storage::set(const std::string& key, const std::string& datum){
        lock.lock();
        data[key] = datum;
        lock.unlock();
    }

}
}
