//
// Created by Tim on 30/12/2021.
//

#ifndef OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_INFLUENCEPRECOMPUTES_H_
#define OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_INFLUENCEPRECOMPUTES_H_
#include "Includes.h"
#include "core/Math.h"

namespace Overseer::Core::Influence
{
    struct InfluencePrecomputes
    {
        inline static float ThreatRangedAttack4Falloff[INFLUENCE_THREAT_SIZE];

        inline static float ThreatRangedAttack3Static[INFLUENCE_THREAT_SIZE];

        InfluencePrecomputes()
        {
            int infIndex = 0;

            for (int y = 0; y < INFLUENCE_THREAT_WIDTH; ++y)
            {
                for (int x = 0; x < INFLUENCE_THREAT_WIDTH; ++x)
                {
                    int distance = max(DistanceChebyshev(INFLUENCE_THREAT_CENTER, int2(x, y)) - 4, 0);
                    ThreatRangedAttack4Falloff[infIndex] =
                        CalculateInverseLinearInfluence(1.0f, distance, INFLUENCE_THREAT_RADIUS - 2);
                    infIndex++;
                }
            }

            infIndex = 0;
            for (int y = 0; y < INFLUENCE_THREAT_WIDTH; ++y)
            {
                for (int x = 0; x < INFLUENCE_THREAT_WIDTH; ++x)
                {
                    int distance                  = DistanceChebyshev(INFLUENCE_THREAT_CENTER, int2(x, y));
                    ThreatRangedAttack3Static[infIndex] = distance > 3 ? 0 : 1;
                    infIndex++;
                }
            }
        }
    };
} // namespace Overseer::Systems::Influence
#endif // OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_INFLUENCEPRECOMPUTES_H_
