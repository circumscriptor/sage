#include "Log.hpp"

#include <Sage/Core/IO/Path.hpp>
#include <fmt/format.h>
#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#ifndef SAGE_GLOBAL_LOG_NAME
    #define SAGE_GLOBAL_LOG_NAME "sage"
#endif

namespace Sage::Core::Console {

Log::Log(std::string name, std::string file, bool isGlobal) : mIsGlobal{isGlobal} {
    auto stdoutSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto fileSink   = std::make_shared<spdlog::sinks::basic_file_sink_mt>(std::move(file), true);

    std::array<spdlog::sink_ptr, 2> sinks = {stdoutSink, fileSink};

    mLogger = std::make_shared<spdlog::logger>(std::move(name), sinks.begin(), sinks.end());
    mLogger->set_level(spdlog::level::trace);
}

Log::Log() : Log(SAGE_GLOBAL_LOG_NAME, std::string(IO::Path::Log()), true) {
    spdlog::register_logger(mLogger);
}

Log::~Log() {
    if (mIsGlobal) {
        spdlog::shutdown();
    }
}

}; // namespace Sage::Core::Console
