#pragma once

#include <iostream>
#include <stdexcept>
#include <string>

namespace isc
{
    class RuntimeException
        : public std::runtime_error
    {
    public:

        RuntimeException(const std::string& message, const std::string& details)
            : std::runtime_error(message + " - " + details)
        {
            std::cout << "[RuntimeException] " << what() << std::endl;
        }
    };
}
