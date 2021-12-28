//
// Created by Tim on 26/12/2021.
//

#ifndef OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_INFLUENCEMAPS_H_
#define OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_INFLUENCEMAPS_H_
#include "Includes.h"
#include "common/Math.h"

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

    struct ProxPrecomputes
    {
        static float Influence[INFLUENCE_SIZE];

        ProxPrecomputes()
        {
            int infIndex = 0;

            for (int y = 0; y < INFLUENCE_WIDTH; ++y)
            {
                for (int x = 0; x < INFLUENCE_WIDTH; ++x)
                {
                    int distance        = DistanceChebyshev(INFLUENCE_CENTER, int2(x, y));
                    Influence[infIndex] = CalculateInverse2PolyInfluence(0.5f, distance, INFLUENCE_RADIUS);
                    infIndex++;
                }
            }
        }
    };

    constexpr static float CalculateAttackLookupValue(int distance)
    {
        float val = min(distance, INFLUENCE_RADIUS) / INFLUENCE_RADIUS;
        return 1.0f - 1.0f * (val * val);
    }

    struct ThreatPrecomputes
    {
        constexpr static float AttackLookup[8] = { CalculateAttackLookupValue(0), CalculateAttackLookupValue(1),
                                                   CalculateAttackLookupValue(2), CalculateAttackLookupValue(3),
                                                   CalculateAttackLookupValue(4), CalculateAttackLookupValue(5),
                                                   CalculateAttackLookupValue(6), CalculateAttackLookupValue(7) };
    };
} // namespace Overseer::Systems

#endif // OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_INFLUENCEMAPS_H_
