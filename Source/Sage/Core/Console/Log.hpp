

#pragma once

#include <Sage/Core/ClassDefinitions.hpp>
#include <memory>
#include <spdlog/spdlog.h>

namespace Sage::Core::Console {

class Log {
  public:

    SAGE_CLASS_DELETE_COPY_AND_MOVE(Log)

    Log(std::string name, std::string file) : Log(std::move(name), std::move(file), false) {}

    ~Log();

    [[nodiscard]] auto Logger() const -> std::shared_ptr<spdlog::logger> {
        return mLogger;
    }

    ///
    /// @brief Get global logger (created on first call)
    ///
    /// @return Global logger
    ///
    static auto Get() -> Log& {
        static Log sLog;
        return sLog;
    }

  private:

    Log();

    Log(std::string name, std::string file, bool isGlobal);

    std::shared_ptr<spdlog::logger> mLogger;
    bool                            mIsGlobal{false};
};

} // namespace Sage::Core::Console

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#if SAGE_LOG_SOURCE
    #define SAGE_LOG(level, ...) SPDLOG_LOGGER_CALL(Sage::Core::Console::Log::Get().Logger(), level, __VA_ARGS__)
#else
    #define SAGE_LOG(level, ...) (Sage::Core::Console::Log::Get().Logger())->log(level, __VA_ARGS__)
#endif

#define SAGE_LOG_TRACE(...)    SAGE_LOG(spdlog::level::trace, __VA_ARGS__)
#define SAGE_LOG_DEBUG(...)    SAGE_LOG(spdlog::level::debug, __VA_ARGS__)
#define SAGE_LOG_INFO(...)     SAGE_LOG(spdlog::level::info, __VA_ARGS__)
#define SAGE_LOG_WARN(...)     SAGE_LOG(spdlog::level::warn, __VA_ARGS__)
#define SAGE_LOG_ERROR(...)    SAGE_LOG(spdlog::level::err, __VA_ARGS__)
#define SAGE_LOG_CRITICAL(...) SAGE_LOG(spdlog::level::critical, __VA_ARGS__)
// NOLINTEND(cppcoreguidelines-macro-usage)
