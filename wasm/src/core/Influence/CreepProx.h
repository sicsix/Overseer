//
// Created by Tim on 1/01/2022.
//

#ifndef OVERSEER_WASM_SRC_CORE_INFLUENCE_CREEPPROX_H_
#define OVERSEER_WASM_SRC_CORE_INFLUENCE_CREEPPROX_H_
#include "LocalMap.h"
#include "core/Structures.h"

namespace Overseer::Core::Influence
{
    struct CreepProx : LocalMap
    {
        float* Influence = new float[INFLUENCE_PROX_SIZE];

        void Calculate(Core::NavMap& navMap)
        {
            int worldIndex = WorldStartIndex;
            int localIndex = LocalStartIndex;

            int width           = LocalEnd.x - LocalStart.x;
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
            for (int y = LocalStart.y; y < LocalEnd.y; ++y)
            {
                for (int x = LocalStart.x; x < LocalEnd.x; ++x)
                {
                    int dist              = DistanceChebyshev(LocalCenter, int2(x, y));
                    dist                  = navMap.Map[worldIndex] == INT_MAXVALUE ? INFLUENCE_PROX_RADIUS : dist;
                    float inf             = CalculateInverseLinearInfluence(1.0f, dist, INFLUENCE_PROX_RADIUS);
                    Influence[localIndex] = inf;
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
}
#endif //OVERSEER_WASM_SRC_CORE_INFLUENCE_CREEPPROX_H_
