//
// Created by Tim on 26/12/2021.
//

#ifndef OVERSEER_WASM_SRC_CORE_INFLUENCEMAPS_H_
#define OVERSEER_WASM_SRC_CORE_INFLUENCEMAPS_H_
#include "Includes.h"
#include "core/Math.h"

namespace Overseer::Core
{
    struct IMAP
    {
        float* Influence = new float[MAP_SIZE];
    };

    struct FriendlyProxIMAP : IMAP
    {
    };

    struct FriendlyThreatIMAP : IMAP
    {
    };

    struct EnemyProxIMAP : IMAP
    {
    };

    struct EnemyThreatIMAP : IMAP
    {
    };
} // namespace Overseer::Systems::Influence

#endif // OVERSEER_WASM_SRC_CORE_INFLUENCEMAPS_H_
