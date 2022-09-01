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
/// @file ImGuiInterface.hpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#pragma once

#include <Sage/Core/ClassDefinitions.hpp>
#include <Sage/Core/Graphics/GraphicsContext.hpp>

union SDL_Event;

namespace Sage::Core::Graphics {

class IImGuiInterface {
  public:

    static constexpr UInt32 kDefaultInitialVBSize = 1024;
    static constexpr UInt32 kDefaultInitialIBSize = 2048;

    SAGE_CLASS_DELETE_COPY_AND_MOVE(IImGuiInterface)

    IImGuiInterface() = default;

    virtual ~IImGuiInterface() = default;

    virtual void NewFrame(float deltaTime) = 0;

    virtual void EndFrame() = 0;

    virtual void Render() = 0;

    virtual void ProcessEvent(const SDL_Event& event) = 0;

    static std::shared_ptr<IImGuiInterface> CreateInstance(std::shared_ptr<IGraphicsContext> graphics,
                                                           UInt32 vertexBufferSize = kDefaultInitialVBSize,
                                                           UInt32 indexBufferSize  = kDefaultInitialIBSize);
};

} // namespace Sage::Core::Graphics
