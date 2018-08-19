#include "UpdateProfiler.hpp"

#include <iostream>
#include <iomanip>
#include <chrono>

namespace isc
{
    UpdateProfiler::UpdateProfiler()
        : _deltaTotal(0)
        , _tickCount(0)
    {
    }

    void UpdateProfiler::update(DeltaTime deltaTime)
    {
        _deltaTotal += deltaTime;
        ++_tickCount;

        report();
    }

    void UpdateProfiler::report()
    {
        using namespace std::chrono_literals;

        if (_deltaTotal >= 1s)
        {
            auto average = _deltaTotal / _tickCount;

            std::cout << "[Profiler] "
                << _tickCount << " fps (~"
                << std::setprecision(3) << average.count() << "ms)"
                << std::endl;

            _deltaTotal -= 1s;
            _tickCount = 0;
        }
    }
}
