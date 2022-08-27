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
/// @file Timer.hpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#pragma once

#include "BasicTypes.hpp"

namespace Sage {

class Timer {
  public:

    Timer();

    void Update();

    // Set update interval (times per second)
    void SetInterval(UInt64 interval) noexcept {
        mInterval = interval;
    }

    [[nodiscard]] UInt64 GetInterval() const noexcept {
        return mInterval;
    }

    [[nodiscard]] Float64 DeltaTime() const noexcept {
        return mDeltaTime;
    }

    [[nodiscard]] Float64 AvgDeltaTime() const noexcept {
        return mAvgDeltaTime;
    }

    [[nodiscard]] Float64 MinDeltaTime() const noexcept {
        return mMinDeltaTime;
    }

    [[nodiscard]] Float64 MaxDeltaTime() const noexcept {
        return mMaxDeltaTime;
    }

    [[nodiscard]] Float64 AvgFrameRate() const noexcept {
        return mAvgFrameRate;
    }

  private:

    UInt64       mLastTime{0};
    const UInt64 mFrequency;
    UInt64       mTimeSinceAverage{0};
    Float64      mAvgValue{0.0};
    Float64      mMinValue{1e+6}; // Large number
    Float64      mMaxValue{0.0};
    UInt32       mAvgCount{0};
    UInt64       mInterval{0};

    Float64 mDeltaTime{0.0};
    UInt64  mDeltaTimeInt{0};
    Float64 mAvgDeltaTime{0.0};
    Float64 mMinDeltaTime{0.0};
    Float64 mMaxDeltaTime{0.0};
    Float64 mAvgFrameRate{0.0};
};

} // namespace Sage
