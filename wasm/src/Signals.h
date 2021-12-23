//
// Created by Tim on 27/12/2021.
//

#ifndef OVERSEER_WASM_SRC_SIGNALS_H_
#define OVERSEER_WASM_SRC_SIGNALS_H_
#include "Includes.h"
#include "entity/registry.hpp"
#include "signal/sigh.hpp"

namespace Overseer
{
    struct Signals
    {
        inline static entt::sigh<void(entt::registry& registry)> TerrainMapImported;
        inline static auto TerrainMapImportedSink = entt::sink(TerrainMapImported);
    };
} // namespace Overseer

#endif //OVERSEER_WASM_SRC_SIGNALS_H_
