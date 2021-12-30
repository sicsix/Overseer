//
// Created by Tim on 27/12/2021.
//

#ifndef OVERSEER_WASM_SRC_CORE_NAVIGATION_H_
#define OVERSEER_WASM_SRC_CORE_NAVIGATION_H_
#include "Includes.h"

namespace Overseer::Core
{
    constexpr static int TerrainCost[] = { 1, INT_MAXVALUE, 5 };

    struct Node
    {
        int CameFrom;
        int Cost = 2147483647;

        Node()
        {
        }

        Node(int cameFrom, int cost): CameFrom(cameFrom), Cost(cost)
        {
        }
    };
} // namespace Overseer::Core

#endif // OVERSEER_WASM_SRC_CORE_NAVIGATION_H_
