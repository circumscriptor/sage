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
/// @file Path.cpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#include "Path.hpp"

#include <Sage/Core/Console/Log.hpp>
#include <Sage/Core/SDL.hpp>
#include <algorithm>

#ifndef SAGE_DEFAULT_BASE_PATH
    #define SAGE_DEFAULT_BASE_PATH ""
#endif

#ifndef SAGE_DEFAULT_USER_PATH
    #define SAGE_DEFAULT_USER_PATH ""
#endif

#ifndef SAGE_GLOBAL_LOG_FILE_NAME
    #define SAGE_GLOBAL_LOG_FILE_NAME "sage.log"
#endif

#ifndef SAGE_GLOBAL_CONFIG_FILE_NAME
    #define SAGE_GLOBAL_CONFIG_FILE_NAME "sage.cfg"
#endif

namespace Sage::Core::IO {

// NOTE: Using '/' instead of '\\', because some dependencies cannot handle '\\'

std::string Path::GetBasePath() {
    const char* basePath = SDL::Get().GetBasePath();
    std::string path     = basePath == nullptr ? SAGE_DEFAULT_BASE_PATH : basePath;
    std::replace(path.begin(), path.end(), '\\', '/');
    return path;
}

std::string Path::GetUserPath() {
    const char* userPath = SDL::Get().GetUserPath();
    std::string path     = userPath == nullptr ? SAGE_DEFAULT_USER_PATH : userPath;
    std::replace(path.begin(), path.end(), '\\', '/');
    return path;
}

std::string_view Path::Base() {
    static const std::string sBasePath = GetBasePath();
    return sBasePath;
}

std::string_view Path::User() {
    static const std::string sUserPath = GetUserPath();
    return sUserPath;
}

std::string_view Path::Log() {
    static const std::string sLogPath = fmt::format("{}{}", User(), SAGE_GLOBAL_LOG_FILE_NAME);
    return sLogPath;
}

std::string_view Path::Config() {
    static const std::string sConfigPath = fmt::format("{}{}", User(), SAGE_GLOBAL_CONFIG_FILE_NAME);
    return sConfigPath;
}

bool Path::IsFile(std::string_view path) {
    return SDL::Get().FileExists(path.data());
}

} // namespace Sage::Core::IO
