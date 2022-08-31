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
/// @file GraphicsContext.hpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#pragma once

#include "GraphicsCVars.hpp"
#include "Sage/Core/Graphics/GraphicsContext.hpp"

#include <Sage/Core/BasicTypes.hpp>
#include <Sage/Core/Console/CVarManager.hpp>
#include <Sage/Core/Console/VirtualConsole.hpp>

// Diligent
#include <DeviceContext.h>
#include <EngineFactory.h>
#include <GraphicsTypes.h>
#include <RefCntAutoPtr.hpp>
#include <RenderDevice.h>
#include <SwapChain.h>

// stdlib
#include <array>

struct SDL_Window;

namespace Sage::Core::Graphics {

class IGraphicsContext {
  public:

    static constexpr std::array<float, 4> kClearColor{0.F, 0.F, 0.F, 1.F}; //!< Default clear color

    SAGE_CLASS_DELETE(IGraphicsContext)

    IGraphicsContext(std::shared_ptr<Console::IVirtualConsole> console, Console::IVirtualConsole::ContextID contextID) :
        mConsole{std::move(console)},
        mContextID{contextID} {
        mConsole->RegisterVolatile(contextID, mCVars);
    }

    virtual ~IGraphicsContext() = default;

    virtual void Clear() = 0;

    virtual void Present() = 0;

    static std::shared_ptr<IGraphicsContext> CreateInstance(std::shared_ptr<Console::IVirtualConsole> console,
                                                            Console::IVirtualConsole::ContextID       contextID,
                                                            std::shared_ptr<IGraphicsContext>         base = nullptr);

  private:

    std::shared_ptr<Console::IVirtualConsole> mConsole;
    const Console::IVirtualConsole::ContextID mContextID;
    GraphicsCVars                             mCVars;

  protected:

    [[nodiscard]] GraphicsCVars& CVars() noexcept {
        return mCVars;
    }
};

} // namespace Sage::Core::Graphics
