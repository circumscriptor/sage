
#include "Timer.hpp"

#include <SDL2/SDL_timer.h>
#include <algorithm>

namespace Sage::Core {

Timer::Timer() : mLastTime{SDL_GetPerformanceCounter()}, mFrequency{SDL_GetPerformanceFrequency()} {}

void Timer::Update() {
    // Compute delta time
    auto currTime = SDL_GetPerformanceCounter();
    mDeltaTimeInt = currTime - mLastTime;
    mDeltaTime    = Float64(mDeltaTimeInt) / Float64(mFrequency);
    mLastTime     = currTime;

    // Per-frame update
    ++mAvgCount;
    mAvgValue = mAvgValue + (mDeltaTime - mAvgValue) / Float64(mAvgCount);
    mTimeSinceAverage += mDeltaTimeInt;
    mMinValue = std::min(mMinValue, mDeltaTime);
    mMaxValue = std::max(mMaxValue, mDeltaTime);

    // Per-interval update
    if (mInterval != 0 && mTimeSinceAverage >= (mFrequency / mInterval)) {
        mAvgDeltaTime = mAvgValue;
        mMinDeltaTime = mMinValue;
        mMaxDeltaTime = mMaxValue;
        mAvgFrameRate = 1.0 / mAvgDeltaTime;

        mAvgValue         = mDeltaTime;
        mMinValue         = mDeltaTime;
        mMaxValue         = mDeltaTime;
        mAvgCount         = 1;
        mTimeSinceAverage = 0;
    }
}

} // namespace Sage::Core
