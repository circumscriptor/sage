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
/// @file Engine.hpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#pragma once

#include "Context.hpp"

#include <list>

namespace Sage::Core {

using namespace Console;
using namespace Graphics;

class Engine {
  public:

    SAGE_CLASS_DELETE_COPY_AND_MOVE(Engine)

    Engine(IVirtualConsole& console);

    ~Engine();

    void Loop();

    static bool Run();

  private:

    void ComputeDelta();

    std::list<Context> mContexts;
    GraphicsCVars      mGraphicsCVars; //!< Global graphics CVars (persistent)
};

} // namespace Sage::Core
