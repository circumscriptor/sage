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
/// @file Enumerate.hpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#pragma once

#include <Sage/Core/BasicTypes.hpp>
#include <Sage/Core/ClassDefinitions.hpp>

// Diligent
#include <EngineFactory.h>

// stdlib
#include <vector>

namespace Sage::Core::Graphics::Internal {

using namespace Diligent;

static inline UInt32
Enumerate(IEngineFactory* factory, Version minVersion, std::vector<GraphicsAdapterInfo>& adapters) {
    UInt32 adaptersCount = 0;
    factory->EnumerateAdapters(minVersion, adaptersCount, nullptr);

    if (adaptersCount > 0) {
        adapters.resize(adaptersCount);
        factory->EnumerateAdapters(minVersion, adaptersCount, adapters.data());
    }
    return adaptersCount;
}

} // namespace Sage::Core::Graphics::Internal
