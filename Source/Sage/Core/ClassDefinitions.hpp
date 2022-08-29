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
/// @file ClassDefinitions.hpp
/// @brief ...
///
/// @copyright Copyright (c) 2022
///
///

#pragma once

#define SAGE_CLASS_CONSTRUCTOR_DEFAULT(ClassName, Definition) ClassName() = Definition;
#define SAGE_CLASS_DESTRUCTOR(ClassName, Definition)          ~ClassName() = Definition;
#define SAGE_CLASS_COPY_CONSTRUCTOR(ClassName, Definition)    ClassName(const ClassName&) = Definition;
#define SAGE_CLASS_MOVE_CONSTRUCTOR(ClassName, Definition)    ClassName(ClassName&&) = Definition;
#define SAGE_CLASS_COPY_ASSIGN(ClassName, Definition)         ClassName& operator=(const ClassName&) = Definition;
#define SAGE_CLASS_MOVE_ASSIGN(ClassName, Definition)         ClassName& operator=(ClassName&&) = Definition;

#define SAGE_CLASS_COPY(ClassName, Definition)                                                                         \
    SAGE_CLASS_COPY_CONSTRUCTOR(ClassName, Definition)                                                                 \
    SAGE_CLASS_COPY_ASSIGN(ClassName, Definition)

#define SAGE_CLASS_MOVE(ClassName, Definition)                                                                         \
    SAGE_CLASS_MOVE_CONSTRUCTOR(ClassName, Definition)                                                                 \
    SAGE_CLASS_MOVE_ASSIGN(ClassName, Definition)

#define SAGE_CLASS_DELETE_COPY_AND_MOVE(ClassName)                                                                     \
    SAGE_CLASS_COPY(ClassName, delete)                                                                                 \
    SAGE_CLASS_MOVE(ClassName, delete)

#define SAGE_CLASS_DEFAULT_COPY_AND_MOVE(ClassName)                                                                    \
    SAGE_CLASS_COPY(ClassName, default)                                                                                \
    SAGE_CLASS_MOVE(ClassName, default)

#define SAGE_CLASS_DEFAULT(ClassName)                                                                                  \
    SAGE_CLASS_CONSTRUCTOR_DEFAULT(ClassName, default)                                                                 \
    SAGE_CLASS_DEFAULT_COPY_AND_MOVE(ClassName)

#define SAGE_CLASS_DELETE(ClassName)                                                                                   \
    SAGE_CLASS_CONSTRUCTOR_DEFAULT(ClassName, delete)                                                                  \
    SAGE_CLASS_DELETE_COPY_AND_MOVE(ClassName)

#define SAGE_CLASS_DEFAULT_ALL(ClassName)                                                                              \
    SAGE_CLASS_DEFAULT(ClassName)                                                                                      \
    SAGE_CLASS_DESTRUCTOR(ClassName, default)
