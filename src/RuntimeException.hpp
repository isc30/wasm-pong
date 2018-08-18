#pragma once

#include <stdexcept>
#include <string>

class RuntimeException
    : public std::runtime_error
{
public:

    RuntimeException(const std::string& message, const std::string& details)
        : std::runtime_error(message + " - " + details)
    {
    }
};