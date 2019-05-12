#pragma once

#include <iostream>
#include <list>
#include <vector>

#include <Engine/Integrations/Emscripten.hpp>

namespace isc
{
    class ResourceProvider
    {
    public:

        bool complete = false;

        void add(const std::string& file)
        {
            _files.push_back(file);
            complete = false;
        }

        void onSuccess(const char* file)
        {
            std::cout << "file ok: " << file << std::endl;
            _files.remove(file);
            complete = _files.size() == 0;
        }

        void onError(const char* file)
        {
            std::cout << "file error: " << file << std::endl;
            _files.remove(file);
            complete = false;
        }

        void prepare()
        {
            std::vector<std::string> cache = { std::begin(_files), std::end(_files) };

            for (const std::string& file : cache)
            {
                emscripten::prepareFile(
                    file.c_str(),
                    [&](const char* arg0) { onSuccess(arg0); },
                    [&](const char* arg0) { onError(arg0); });
            }
        }

    private:

        std::list<std::string> _files;
    };
}
