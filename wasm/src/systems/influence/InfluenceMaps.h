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
        inline static float Influence[INFLUENCE_SIZE];

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

    constexpr static float CalculateAttackLookupValue(float distance)
    {
        float val = min(distance, INFLUENCE_RADIUS) / INFLUENCE_RADIUS;
        return 1.0f - val; // Linear
        // return 1.0f - (val * val);             // 2 Poly
        // return 1.0f - (val * val * val * val); // 4 Poly
    }

    struct ThreatPrecomputes
    {
        constexpr static float AttackLookup[8] = { CalculateAttackLookupValue(0.0f), CalculateAttackLookupValue(1.0f),
                                                   CalculateAttackLookupValue(2.0f), CalculateAttackLookupValue(3.0f),
                                                   CalculateAttackLookupValue(4.0f), CalculateAttackLookupValue(5.0f),
                                                   CalculateAttackLookupValue(6.0f), CalculateAttackLookupValue(7.0f) };

        inline static float RangedAttack4Falloff[INFLUENCE_SIZE];

        inline static float RangedAttack3Static[INFLUENCE_SIZE];

        ThreatPrecomputes()
        {
            int infIndex = 0;

            for (int y = 0; y < INFLUENCE_WIDTH; ++y)
            {
                for (int x = 0; x < INFLUENCE_WIDTH; ++x)
                {
                    int distance                   = max(DistanceChebyshev(INFLUENCE_CENTER, int2(x, y)) - 4, 0);
                    RangedAttack4Falloff[infIndex] = CalculateInverseLinearInfluence(1.0f, distance, INFLUENCE_RADIUS - 2);
                    infIndex++;
                }
            }

            infIndex = 0;
            for (int y = 0; y < INFLUENCE_WIDTH; ++y)
            {
                for (int x = 0; x < INFLUENCE_WIDTH; ++x)
                {
                    int distance                   = DistanceChebyshev(INFLUENCE_CENTER, int2(x, y));
                    RangedAttack3Static[infIndex] = distance > 3 ? 0 : 1;
                    infIndex++;
                }
            }
        }
    };
} // namespace Overseer::Systems

#endif // OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_INFLUENCEMAPS_H_
