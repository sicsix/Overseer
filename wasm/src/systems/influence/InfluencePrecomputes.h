//
// Created by Tim on 30/12/2021.
//

#ifndef OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_INFLUENCEPRECOMPUTES_H_
#define OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_INFLUENCEPRECOMPUTES_H_
#include "Includes.h"
#include "core/Math.h"

namespace Overseer::Systems::Influence
{
    constexpr static float CalculateLinearLookup(float distance)
    {
        float val = min(distance, INFLUENCE_RADIUS) / INFLUENCE_RADIUS;
        return 1.0f - val; // Linear
        // return 1.0f - (val * val);             // 2 Poly
        // return 1.0f - (val * val * val * val); // 4 Poly
    }

    struct InfluencePrecomputes
    {
        constexpr static float LinearLookup[8] = { CalculateLinearLookup(0.0f), CalculateLinearLookup(1.0f),
                                                   CalculateLinearLookup(2.0f), CalculateLinearLookup(3.0f),
                                                   CalculateLinearLookup(4.0f), CalculateLinearLookup(5.0f),
                                                   CalculateLinearLookup(6.0f), CalculateLinearLookup(7.0f) };

        inline static float RangedAttack4Falloff[INFLUENCE_SIZE];

        inline static float RangedAttack3Static[INFLUENCE_SIZE];

        InfluencePrecomputes()
        {
            int infIndex = 0;

            for (int y = 0; y < INFLUENCE_WIDTH; ++y)
            {
                for (int x = 0; x < INFLUENCE_WIDTH; ++x)
                {
                    int distance = max(DistanceChebyshev(INFLUENCE_CENTER, int2(x, y)) - 4, 0);
                    RangedAttack4Falloff[infIndex] =
                        CalculateInverseLinearInfluence(1.0f, distance, INFLUENCE_RADIUS - 2);
                    infIndex++;
                }
            }

            infIndex = 0;
            for (int y = 0; y < INFLUENCE_WIDTH; ++y)
            {
                for (int x = 0; x < INFLUENCE_WIDTH; ++x)
                {
                    int distance                  = DistanceChebyshev(INFLUENCE_CENTER, int2(x, y));
                    RangedAttack3Static[infIndex] = distance > 3 ? 0 : 1;
                    infIndex++;
                }
            }
        }
    }
} // namespace Overseer::Systems::Influence
#endif // OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_INFLUENCEPRECOMPUTES_H_
