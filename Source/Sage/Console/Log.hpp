

#pragma once

#include <memory>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

namespace Sage::Console {

class Log {
  public:

    // TODO: Pass file location
    static void Initialize();

    static void Shutdown();

    static std::shared_ptr<spdlog::logger> Get() {
        return sLog;
    }

  private:

    static std::shared_ptr<spdlog::logger> sLog;
};

} // namespace Sage::Console

#if SAGE_LOG_SOURCE
    #define SAGE_LOG(level, ...) SPDLOG_LOGGER_CALL(Sage::Console::Log::Get(), level, __VA_ARGS__)
#else
    #define SAGE_LOG(level, ...) (Sage::Console::Log::Get())->log(level, __VA_ARGS__)
#endif

#define SAGE_LOG_TRACE(...)    SAGE_LOG(spdlog::level::trace, __VA_ARGS__)
#define SAGE_LOG_DEBUG(...)    SAGE_LOG(spdlog::level::debug, __VA_ARGS__)
#define SAGE_LOG_INFO(...)     SAGE_LOG(spdlog::level::info, __VA_ARGS__)
#define SAGE_LOG_WARN(...)     SAGE_LOG(spdlog::level::warn, __VA_ARGS__)
#define SAGE_LOG_ERROR(...)    SAGE_LOG(spdlog::level::err, __VA_ARGS__)
#define SAGE_LOG_CRITICAL(...) SAGE_LOG(spdlog::level::critical, __VA_ARGS__)
