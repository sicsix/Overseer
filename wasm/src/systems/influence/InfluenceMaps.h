//
// Created by Tim on 26/12/2021.
//

#ifndef OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_INFLUENCEMAPS_H_
#define OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_INFLUENCEMAPS_H_
#include "Includes.h"

namespace Overseer::Systems
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

    struct ProxSplatMap
    {
        float* Influence = new float[INFLUENCE_SIZE];
    };
} // namespace Overseer::Systems

#endif // OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_INFLUENCEMAPS_H_
