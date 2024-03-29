//
// Created by Tim on 1/01/2022.
//

#ifndef OVERSEER_WASM_SRC_CORE_INFLUENCE_LOCALMAP_H_
#define OVERSEER_WASM_SRC_CORE_INFLUENCE_LOCALMAP_H_
#include "Includes.h"
#include "core/Math.h"

namespace Overseer::Core::Influence
{
    struct LocalMap
    {
        int2 WorldStart;
        int2 WorldEnd;
        int2 WorldCenter;
        int  WorldCenterIndex;
        int  WorldStartIndex;
        int2 LocalStart;
        int2 LocalEnd;
        int2 LocalCenter;
        int  LocalCenterIndex;
        int  LocalStartIndex;

        void ClampAndSetBounds(int2 center, int localMapRadius, int localMapWidth)
        {
            int2 worldStartUnclamped = center - localMapRadius;
            int2 worldEndUnclamped   = center + localMapRadius + 1;

            WorldStart       = max(worldStartUnclamped, 0);
            WorldEnd         = min(worldEndUnclamped, MAP_WIDTH);
            WorldCenter      = center;
            WorldCenterIndex = PosToIndex(WorldCenter, MAP_WIDTH);
            WorldStartIndex  = PosToIndex(WorldStart, MAP_WIDTH);
            LocalStart       = max(WorldStart - worldStartUnclamped, 0);
            LocalEnd         = localMapWidth - (worldEndUnclamped - WorldEnd);
            LocalCenter      = int2(localMapRadius, localMapRadius);
            LocalCenterIndex = PosToIndex(LocalCenter, localMapWidth);
            LocalStartIndex  = PosToIndex(LocalStart, localMapWidth);

            // printf(
            //     "WorldCenter: { %i, %i }    WorldStart: { %i, %i }    WorldEnd: { %i, %i }   WorldStartIndex: % i    LocalCenter: { %i, %i }    LocalStart: { %i, %i }    LocalEnd: { %i, %i }   LocalStartIndex: % i\n",
            //     WorldCenter.x,
            //     WorldCenter.y,
            //     WorldStart.x,
            //     WorldStart.y,
            //     WorldEnd.x,
            //     WorldEnd.y,
            //     WorldStartIndex,
            //     LocalCenter.x,
            //     LocalCenter.y,
            //     LocalStart.x,
            //     LocalStart.y,
            //     LocalEnd.x,
            //     LocalEnd.y,
            //     LocalStartIndex);
        }
    };
}
#endif //OVERSEER_WASM_SRC_CORE_INFLUENCE_LOCALMAP_H_
