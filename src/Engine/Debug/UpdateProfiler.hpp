#pragma once

#include <Engine/Common.hpp>

namespace isc
{
    class UpdateProfiler
    {
    public:

        UpdateProfiler();
        void update(DeltaTime deltaTime);
        void report();

    private:

        DeltaTime _deltaTotal;
        size_t _tickCount;
    };
}
