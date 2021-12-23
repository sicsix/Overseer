//
// Created by Tim on 25/12/2021.
//

#ifndef OVERSEER_WASM_SRC_IMPORTS_EXPORTFLAGS_H_
#define OVERSEER_WASM_SRC_IMPORTS_EXPORTFLAGS_H_

namespace Overseer::Imports::ExportFlagsNS
{
    enum ExportFlags : uint
    {
        NONE      = 1 << 0,
        BEGIN     = 1 << 1,
        END       = 1 << 2,
        NEXT      = 1 << 3,
//        NEW       = 1 << 4,
        DESTROYED = 1 << 5,
        MAP       = 1 << 6,
        CREEP     = 1 << 7
    };
} // namespace Overseer::Imports::ExportFlagsNS

#endif //OVERSEER_WASM_SRC_IMPORTS_EXPORTFLAGS_H_
