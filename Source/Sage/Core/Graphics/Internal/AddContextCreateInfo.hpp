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
/// @file AddContextCreateInfo.hpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#pragma once

// Diligent
#include <GraphicsTypes.h>

// stdlib
#include <vector>

namespace Sage::Core::Graphics::Internal {

static inline bool AddContextCreateInfo(std::vector<Diligent::ImmediateContextCreateInfo>& contextCIs,
                                        Diligent::GraphicsAdapterInfo&                     adapter,
                                        const char*                                        name,
                                        Diligent::COMMAND_QUEUE_TYPE                       queueType,
                                        Diligent::QUEUE_PRIORITY                           queuePriority) {
    auto& queues = adapter.Queues;
    for (Diligent::Uint32 i = 0, count = adapter.NumQueues; i < count; ++i) {
        auto& queue = queues[i];

        if (queue.MaxDeviceContexts == 0) {
            continue;
        }

        if ((queue.QueueType & Diligent::COMMAND_QUEUE_TYPE_PRIMARY_MASK) == queueType) {
            queue.MaxDeviceContexts -= 1;
            contextCIs.emplace_back(name, Diligent::Uint8(i), queuePriority);
            return true;
        }
    }
    return false;
}

} // namespace Sage::Core::Graphics::Internal
