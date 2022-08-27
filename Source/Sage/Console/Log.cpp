#include "Log.hpp"

#include <Sage/IO/Path.hpp>
#include <fmt/format.h>
#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#ifndef SAGE_LOG_FILE_NAME
    #define SAGE_LOG_FILE_NAME "sage.log"
#endif

namespace Sage::Console {

std::shared_ptr<spdlog::logger> Log::sLog;

void Log::Initialize() {
    std::string file = fmt::format("{}{}", Sage::IO::Path::User(), SAGE_LOG_FILE_NAME);

    auto stdoutSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto fileSink   = std::make_shared<spdlog::sinks::basic_file_sink_mt>(file, true);

    std::array<spdlog::sink_ptr, 2> sinks = {stdoutSink, fileSink};

    sLog = std::make_shared<spdlog::logger>("sage", sinks.begin(), sinks.end());
    sLog->set_level(spdlog::level::trace);

    spdlog::register_logger(sLog);
}

void Log::Shutdown() {
    sLog.reset();

    spdlog::shutdown();
}

}; // namespace Sage::Console
