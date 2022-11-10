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

#include <algorithm>
#include <bits/chrono.h>
#include <chrono>
#include <cstdint>

namespace Sage::Core {

class Timer {
  public:

    Timer() : mLast{std::chrono::high_resolution_clock::now()} {}

    void Tick() noexcept {
        auto current = std::chrono::high_resolution_clock::now();
        mDeltaTime   = std::chrono::duration<double, std::milli>(current - mLast).count();
        mLast        = current;
    }

  private:

    double                                         mDeltaTime{0.0};
    std::chrono::high_resolution_clock::time_point mLast;
};

} // namespace Sage::Core
