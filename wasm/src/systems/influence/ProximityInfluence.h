//
// Created by Tim on 30/12/2021.
//

#ifndef OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_PROXIMITYINFLUENCE_H_
#define OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_PROXIMITYINFLUENCE_H_
#include "core/Structures.h"
#include "components/Components.h"
#include "core/Math.h"
#include "core/MovementCoster.h"
#include "core/LineOfSight.h"

namespace Overseer::Systems::Influence
{
    struct ProximityInfluence
    {
        static void Calculate(CreepProxIMAP& proxIMAP, Core::NavMap& navMap)
        {
            int worldIndex = proxIMAP.WorldStartIndex;
            int localIndex = proxIMAP.LocalStartIndex;

            int width           = proxIMAP.LocalEnd.x - proxIMAP.LocalStart.x;
            int worldYIncrement = MAP_WIDTH - width;
            int localYIncrement = INFLUENCE_PROX_WIDTH - width;

            // printf(
            //     "WorldStart: { %i, %i }    WorldEnd: { %i, %i }    WorldCenter: { %i, %i }    WorldStartIndex: %i    WorldYIncrement: %i    LocalStart: { %i, %i }    LocalEnd: { %i, %i }    LocalStartIndex: %i    LocalYIncrement: %i\n",
            //     proxIMAP.WorldStart.x,
            //     proxIMAP.WorldStart.y,
            //     proxIMAP.WorldEnd.x,
            //     proxIMAP.WorldEnd.y,
            //     proxIMAP.WorldCenter.x,
            //     proxIMAP.WorldCenter.y,
            //     proxIMAP.WorldStartIndex,
            //     worldYIncrement,
            //     proxIMAP.LocalStart.x,
            //     proxIMAP.LocalStart.y,
            //     proxIMAP.LocalEnd.x,
            //     proxIMAP.LocalEnd.y,
            //     proxIMAP.LocalStartIndex,
            //     localYIncrement);

            // printf("CalculateProximityInfluence: { ");
            for (int y = proxIMAP.LocalStart.y; y < proxIMAP.LocalEnd.y; ++y)
            {
                for (int x = proxIMAP.LocalStart.x; x < proxIMAP.LocalEnd.x; ++x)
                {
                    int dist  = DistanceChebyshev(proxIMAP.LocalCenter, int2(x, y));
                    dist      = navMap.Map[worldIndex] == INT_MAXVALUE ? INFLUENCE_PROX_RADIUS : dist;
                    float inf = CalculateInverseLinearInfluence(1.0f, dist, INFLUENCE_PROX_RADIUS);
                    proxIMAP.Influence[localIndex] = inf;
                    // printf("{ WorldIndex: %i, LocalIndex: %i, Inf: %f }, ", worldIndex, localIndex, inf);
                    worldIndex++;
                    localIndex++;
                }
                worldIndex += worldYIncrement;
                localIndex += localYIncrement;
            }
            // printf("} \n");
        }
    };
} // namespace Overseer::Systems::Influence
#endif // OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_PROXIMITYINFLUENCE_H_
