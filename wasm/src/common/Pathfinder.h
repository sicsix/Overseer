//
// Created by Tim on 26/12/2021.
//

#ifndef OVERSEER_WASM_SRC_COMMON_PATHFINDER_H_
#define OVERSEER_WASM_SRC_COMMON_PATHFINDER_H_
#include "Includes.h"
#include "Structures.h"
#include "components/Components.h"
#include "PriorityQueue.h"

namespace Overseer::Common
{
    class Pathfinder
    {
      private:
        struct FinderNode;

        NavMap                   NavigationMap;
        FinderNode*              BlankNodeSet;
        FinderNode*              NodeSet;
        PriorityQueue<int, Node> PriorityQueue;

      public:
        Pathfinder(NavMap& navMap);

        bool FindPath(int2& start, int2& goal, Components::Path& path);

      private:
        static bool FindPath(NavMap&                           navMap,
                             int2&                             start,
                             int2&                             goal,
                             Path&                             path,
                             Common::PriorityQueue<int, Node>& openSet,
                             FinderNode*                       nodeSet);

        static void ProcessDirections(NavMap&                                     navMap,
                                      int2&                                       pos,
                                      int2&                                       goal,
                                      int                                         index,
                                      int                                         parentCost,
                                      Overseer::Common::PriorityQueue<int, Node>& openSet,
                                      FinderNode*                                 nodeSet,
                                      int4x2                                      offsets);
    };
} // namespace Overseer::Common

#endif // OVERSEER_WASM_SRC_COMMON_PATHFINDER_H_
