//
// Created by Tim on 30/12/2021.
//

#ifndef OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_PROXIMITYINFLUENCE_H_
#define OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_PROXIMITYINFLUENCE_H_
#include "core/Structures.h"
#include "components/Components.h"
#include "core/Math.h"
#include "core/Pathcoster.h"
#include "core/LineOfSight.h"

namespace Overseer::Systems::Influence
{
    static void CalculateProximityInfluence(CreepProxIMAP& proxIMAP, int* costs)
    {
        // printf(
        //     "WorldStart: { %i, %i }    WorldEnd: { %i, %i }    WorldCenter: { %i, %i }    MapStartIndex: %i
        //     MapYIncrement: %i    InfStart: { %i, %i }    InfEnd: { %i, %i }    InfStartIndex: %i InfYIncrement:
        //     %i\n", proxIMAP.WorldStart.x, proxIMAP.WorldStart.y, proxIMAP.WorldEnd.x, proxIMAP.WorldEnd.y,
        //     proxIMAP.WorldCenter.x,
        //     proxIMAP.WorldCenter.y,
        //     proxIMAP.MapStartIndex,
        //     proxIMAP.MapYIncrement,
        //     proxIMAP.InfStart.x,
        //     proxIMAP.InfStart.y,
        //     proxIMAP.InfEnd.x,
        //     proxIMAP.InfEnd.y,
        //     proxIMAP.InfStartIndex,
        //     proxIMAP.InfYIncrement);

        int mapIndex = proxIMAP.MapStartIndex;
        int infIndex = proxIMAP.InfStartIndex;

        // printf("CalculateProximityInfluence: { ");
        for (int y = proxIMAP.InfStart.y; y < proxIMAP.InfEnd.y; ++y)
        {
            for (int x = proxIMAP.InfStart.x; x < proxIMAP.InfEnd.x; ++x)
            {
                int   tileCost               = min(costs[infIndex], 7);
                float inf                    = CalculateLinearLookup(tileCost);
                proxIMAP.Influence[infIndex] = tileCost == INT_MAXVALUE ? 0 : inf;
                // printf("{ MapIndex: %i, MyIndex: %i, Inf: %f }, ", mapIndex, infIndex, inf);
                mapIndex++;
                infIndex++;
            }
            mapIndex += proxIMAP.MapYIncrement;
            infIndex += proxIMAP.InfYIncrement;
        }
        // printf("} \n");
    }
}
#endif // OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_PROXIMITYINFLUENCE_H_
