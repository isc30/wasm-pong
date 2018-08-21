#pragma once

#include <cstdio>

namespace isc
{
    namespace FileSystem
    {
        inline bool fileExists(const char* path)
        {
#ifdef __EMSCRIPTEN__
            // no way to check files before preloading
            return true;
#else
            auto* file = fopen(path, "r");

            if (file != nullptr)
            {
                fclose(file);

                return true;
            }

            return false;
#endif
        }
    }
}
