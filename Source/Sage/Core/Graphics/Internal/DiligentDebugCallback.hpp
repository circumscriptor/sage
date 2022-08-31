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
/// @file DiligentDebugCallback.hpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#pragma once

#include <Sage/Core/Console/Log.hpp>

// Diligent
#include <DebugOutput.h>

namespace Sage::Core::Graphics::Internal {

static inline spdlog::level::level_enum DebugMessageSeverityToLogLevel(Diligent::DEBUG_MESSAGE_SEVERITY severity) {
    switch (severity) {
        case Diligent::DEBUG_MESSAGE_SEVERITY_INFO:
            return spdlog::level::info;
        case Diligent::DEBUG_MESSAGE_SEVERITY_WARNING:
            return spdlog::level::warn;
        case Diligent::DEBUG_MESSAGE_SEVERITY_ERROR:
            return spdlog::level::err;
        case Diligent::DEBUG_MESSAGE_SEVERITY_FATAL_ERROR:
            return spdlog::level::critical;
    }
    return spdlog::level::debug;
}

static void DiligentDebugCallback(enum Diligent::DEBUG_MESSAGE_SEVERITY severity,
                                  const char*                           message,
                                  [[maybe_unused]] const char*          function,
                                  [[maybe_unused]] const char*          file,
                                  [[maybe_unused]] int                  line) {
    auto level = DebugMessageSeverityToLogLevel(severity);
    if (function != nullptr && file != nullptr) {
        SAGE_LOG(level, "[diligent] [{}:{}] ({}) {}", file, line, function, message);
    } else {
        SAGE_LOG(level, "[diligent] {}", message);
    }
}

} // namespace Sage::Core::Graphics::Internal
