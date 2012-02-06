#ifndef HTTPHEADERS_H
#define HTTPHEADERS_H

#include <string>
#include <vector>
namespace Epyx {
    class HTTPHeaders //This class is to complete, perhaps to inherit From another to easy GTP implementation
    {
        public:
            //HTTPHeaders();
            static std::string stripHeaders(std::string message);
            static std::string getHeaders(std::string message);
            // static std::vector<std::string> separateHTTPPackages(std::string messages); //To Implement.
    };
}

#endif // HTTPHEADERS_H
