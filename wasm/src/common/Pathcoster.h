//
// Created by Tim on 28/12/2021.
//

#ifndef OVERSEER_WASM_SRC_COMMON_PATHCOSTER_H_
#define OVERSEER_WASM_SRC_COMMON_PATHCOSTER_H_
#include "Includes.h"
#include "Structures.h"
#include "components/Components.h"
#include "PriorityQueue.h"

namespace Overseer::Common
{
    class Pathcoster
    {
      private:
        struct CostNode;

        NavMap                   NavigationMap;
        CostNode*                BlankNodeSet;
        CostNode*                NodeSet;
        PriorityQueue<int, Node> PriorityQueue;

      public:
        Pathcoster(NavMap& navMap);

        bool GetCosts(int2& start, int2& goal, Components::Path& path);
    };
} // namespace Overseer::Common

#endif // OVERSEER_WASM_SRC_COMMON_PATHCOSTER_H_
