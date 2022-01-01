//
// Created by Tim on 1/01/2022.
//

#ifndef OVERSEER_WASM_SRC_CORE_PROXIMAP_H_
#define OVERSEER_WASM_SRC_CORE_PROXIMAP_H_
#include "Includes.h"
#include "IMAP.h"
#include "CreepProx.h"

namespace Overseer::Core::Influence
{
    struct ProxIMAP : IMAP
    {
        void Add(CreepProx creepProx)
        {
            int worldIndex = creepProx.WorldStartIndex;
            int localIndex = creepProx.LocalStartIndex;

            int width           = creepProx.LocalEnd.x - creepProx.LocalStart.x;
            int worldYIncrement = MAP_WIDTH - width;
            int localYIncrement = INFLUENCE_PROX_WIDTH - width;

            // printf("AddProxInfluence: { ");
            for (int y = creepProx.LocalStart.y; y < creepProx.LocalEnd.y; ++y)
            {
                for (int x = creepProx.LocalStart.x; x < creepProx.LocalEnd.x; ++x)
                {
                    Influence[worldIndex] += creepProx.Influence[localIndex];
                    // printf("{ Influence: %f, MapIndex: %i, InfIndex: %i }",
                    //        creepProx.Influence[infIndex],
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

        void Subtract(CreepProx creepProx)
        {
            int worldIndex = creepProx.WorldStartIndex;
            int localIndex = creepProx.LocalStartIndex;

            int width           = creepProx.LocalEnd.x - creepProx.LocalStart.x;
            int worldYIncrement = MAP_WIDTH - width;
            int localYIncrement = INFLUENCE_PROX_WIDTH - width;

            // printf("SubtractProxInfluence: { ");
            for (int y = creepProx.LocalStart.y; y < creepProx.LocalEnd.y; ++y)
            {
                for (int x = creepProx.LocalStart.x; x < creepProx.LocalEnd.x; ++x)
                {
                    Influence[worldIndex] -= creepProx.Influence[localIndex];
                    // printf("{ Influence: %f, MapIndex: %i, InfIndex: %i }",
                    //        creepProx.Influence[infIndex],
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

    struct FriendlyProxIMAP : ProxIMAP
    {
    };

    struct EnemyProxIMAP : ProxIMAP
    {
    };

} // namespace Overseer::Core::Influence
#endif //OVERSEER_WASM_SRC_CORE_PROXIMAP_H_
