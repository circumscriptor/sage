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
/// @file ModifyCreateInfo.hpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#pragma once

#include "AddContextCreateInfo.hpp"

namespace Sage::Core::Graphics::Internal {

static inline void ModifyCreateInfo(Diligent::RENDER_DEVICE_TYPE                       deviceType,
                                    Diligent::EngineCreateInfo&                        engineCI,
                                    Diligent::SwapChainDesc&                           swapchainDesc,
                                    std::vector<Diligent::GraphicsAdapterInfo>&        adapters,
                                    std::vector<Diligent::ImmediateContextCreateInfo>& contextCIs) {
    (void) deviceType;
    (void) swapchainDesc;

#if PLATFORM_MACOS
    swapchainDesc.BufferCount = 3;
#endif

    // Select adapter with most queues (prefer discrete adapters)
    {
        engineCI.AdapterId         = 0;
        Diligent::Uint32 maxQueues = 0;
        for (Diligent::Uint32 adapterId = 0, count = Diligent::Uint32(adapters.size()); adapterId < count;
             ++adapterId) {
            auto& adapter = adapters[adapterId];

            if (adapter.NumQueues < maxQueues ||
                (adapter.NumQueues == maxQueues && adapter.Type != Diligent::ADAPTER_TYPE_DISCRETE)) {
                continue;
            }

            engineCI.AdapterId = adapterId;
            maxQueues          = adapter.NumQueues;
        }
    }

    auto& adapter = adapters[engineCI.AdapterId];

    AddContextCreateInfo(contextCIs,
                         adapter,
                         "Graphics",
                         Diligent::COMMAND_QUEUE_TYPE_GRAPHICS,
                         Diligent::QUEUE_PRIORITY_HIGH);

    // TODO: Add validation macro

    if (adapter.Type == Diligent::ADAPTER_TYPE_DISCRETE) { // TODO: add force option
        AddContextCreateInfo(contextCIs,
                             adapter,
                             "Transfer",
                             Diligent::COMMAND_QUEUE_TYPE_TRANSFER,
                             Diligent::QUEUE_PRIORITY_MEDIUM);
        AddContextCreateInfo(contextCIs,
                             adapter,
                             "Compute",
                             Diligent::COMMAND_QUEUE_TYPE_COMPUTE,
                             Diligent::QUEUE_PRIORITY_MEDIUM);
    }

    engineCI.Features.NativeFence                   = Diligent::DEVICE_FEATURE_STATE_OPTIONAL;
    engineCI.Features.TimestampQueries              = Diligent::DEVICE_FEATURE_STATE_OPTIONAL;
    engineCI.Features.TransferQueueTimestampQueries = Diligent::DEVICE_FEATURE_STATE_OPTIONAL;
}

} // namespace Sage::Core::Graphics::Internal
