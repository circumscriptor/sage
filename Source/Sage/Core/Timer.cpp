//
//  .oooooo..o       .o.         .oooooo.    oooooooooooo
// d8P'    `Y8      .888.       d8P'  `Y8b   `888'     `8
// Y88bo.          .8"888.     888            888
//  `"Y8888o.     .8' `888.    888            888oooo8
//      `"Y88b   .88ooo8888.   888     ooooo  888    "
// oo     .d8P  .8'     `888.  `88.    .88'   888       o
// 8""88888P'  o88o     o8888o  `Y8bood8P'   o888ooooood8
//

///
/// @file Timer.cpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#include "Timer.hpp"

#include <SDL2/SDL_timer.h>
#include <algorithm>

namespace Sage::Core {

Timer::Timer() : mLastTime{SDL_GetPerformanceCounter()}, mFrequency{SDL_GetPerformanceFrequency()} {}

void Timer::BeginTime() {
    // Compute delta time
    mRenderBegin  = SDL_GetPerformanceCounter();
    mDeltaTimeInt = mRenderBegin - mLastTime;
    mDeltaTime    = Float64(mDeltaTimeInt) / Float64(mFrequency);
    mLastTime     = mRenderBegin;

    // Per-frame update
    ++mAvgCount;
    mAvgValue += (mDeltaTime - mAvgValue) / Float64(mAvgCount);
    mTimeSinceAverage += mDeltaTimeInt;
    mMinValue = std::min(mMinValue, mDeltaTime);
    mMaxValue = std::max(mMaxValue, mDeltaTime);

    // Per-interval update
    if (mInterval != 0 && mTimeSinceAverage >= (mFrequency / mInterval)) {
        mAvgDeltaTime       = mAvgValue;
        mMinDeltaTime       = mMinValue;
        mMaxDeltaTime       = mMaxValue;
        mAvgFrameRate       = 1.0 / mAvgDeltaTime;
        mRenderAvgDeltaTime = mRenderAvgValue;
        mRenderMinDeltaTime = mRenderMinValue;
        mRenderMaxDeltaTime = mRenderMaxValue;

        mAvgValue         = mDeltaTime;
        mMinValue         = mDeltaTime;
        mMaxValue         = mDeltaTime;
        mRenderAvgValue   = mRenderDeltaTime;
        mRenderMinValue   = mRenderDeltaTime;
        mRenderMaxValue   = mRenderDeltaTime;
        mAvgCount         = 1;
        mRenderAvgCount   = 1;
        mTimeSinceAverage = 0;
    }
}

void Timer::EndTime() {
    auto renderEnd   = SDL_GetPerformanceCounter();
    auto timeDiff    = renderEnd - mRenderBegin;
    mRenderDeltaTime = Float64(timeDiff) / Float64(mFrequency);

    ++mRenderAvgCount;
    mRenderAvgValue += (mRenderDeltaTime - mRenderAvgValue) / Float64(mRenderAvgCount);
    mRenderMinValue = std::min(mRenderMinValue, mRenderDeltaTime);
    mRenderMaxValue = std::max(mRenderMaxValue, mRenderDeltaTime);
}

} // namespace Sage::Core
