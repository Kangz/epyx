#include "storage.h"

namespace Epyx
{
namespace DHT
{

    Storage::Storage(){};

    bool Storage::has(const std::string& key){
        return data.count(key) != 0;
    }

    std::string Storage::get(const std::string& key){
        return data[key];
    }

    void Storage::set(const std::string& key, const std::string& datum){
        data[key] = datum;
    }

}
}
