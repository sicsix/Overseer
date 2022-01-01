//
// Created by Tim on 1/01/2022.
//

#ifndef OVERSEER_WASM_SRC_CORE_INFLUENCE_THREATIMAP_H_
#define OVERSEER_WASM_SRC_CORE_INFLUENCE_THREATIMAP_H_
#include "Includes.h"
#include "IMAP.h"
#include "CreepThreat.h"

namespace Overseer::Core::Influence
{
    struct ThreatIMAP : IMAP
    {
        void Add(CreepThreat creepThreat)
        {
            int worldIndex = creepThreat.WorldStartIndex;
            int localIndex = creepThreat.LocalStartIndex;

            int width           = creepThreat.LocalEnd.x - creepThreat.LocalStart.x;
            int worldYIncrement = MAP_WIDTH - width;
            int localYIncrement = INFLUENCE_THREAT_WIDTH - width;

            // printf("AddThreatInfluence: { ");
            for (int y = creepThreat.LocalStart.y; y < creepThreat.LocalEnd.y; ++y)
            {
                for (int x = creepThreat.LocalStart.x; x < creepThreat.LocalEnd.x; ++x)
                {
                    Influence[worldIndex] += creepThreat.Influence[localIndex];
                    // printf("{ Influence: %f, MapIndex: %i, InfIndex: %i }",
                    //        creepThreat.Influence[infIndex],
                    //        mapIndex,
                    //        infIndex);
                    worldIndex++;
                    localIndex++;
                }
                worldIndex += worldYIncrement;
                localIndex += localYIncrement;
            }
            // printf("} \n");
        }

        void Subtract(CreepThreat creepThreat)
        {
            int worldIndex = creepThreat.WorldStartIndex;
            int localIndex = creepThreat.LocalStartIndex;

            int width           = creepThreat.LocalEnd.x - creepThreat.LocalStart.x;
            int worldYIncrement = MAP_WIDTH - width;
            int localYIncrement = INFLUENCE_THREAT_WIDTH - width;

            // printf("AddThreatInfluence: { ");
            for (int y = creepThreat.LocalStart.y; y < creepThreat.LocalEnd.y; ++y)
            {
                for (int x = creepThreat.LocalStart.x; x < creepThreat.LocalEnd.x; ++x)
                {
                    Influence[worldIndex] -= creepThreat.Influence[localIndex];
                    // printf("{ Influence: %f, MapIndex: %i, InfIndex: %i }",
                    //        creepThreat.Influence[infIndex],
                    //        mapIndex,
                    //        infIndex);
                    worldIndex++;
                    localIndex++;
                }
                worldIndex += worldYIncrement;
                localIndex += localYIncrement;
            }
            // printf("} \n");
        }
    };

    struct FriendlyThreatIMAP : ThreatIMAP
    {
    };

    struct EnemyThreatIMAP : ThreatIMAP
    {
    };
} // namespace Overseer::Core::Influence
#endif //OVERSEER_WASM_SRC_CORE_INFLUENCE_THREATIMAP_H_
