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
/// @file Path.hpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#pragma once

#include <string>

namespace Sage::Core::IO {

class Path {
  public:

    static std::string_view Base();

    static std::string_view User();

    static std::string_view Log();

    static std::string_view Config();

    static bool IsFile(std::string_view path);

  private:

    static std::string GetBasePath();

    static std::string GetUserPath();
};

} // namespace Sage::Core::IO
