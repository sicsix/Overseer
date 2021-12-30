//
// Created by Tim on 26/12/2021.
//

#ifndef OVERSEER_WASM_SRC_CORE_PATHFINDER_H_
#define OVERSEER_WASM_SRC_CORE_PATHFINDER_H_
#include "Includes.h"
#include "Structures.h"
#include "components/Components.h"
#include "PriorityQueue.h"

namespace Overseer::Core
{
    class Pathfinder
    {
      private:
        NavMap                        NavigationMap;
        Node*                         BlankNodeSet;
        Node*                         NodeSet;
        PriorityQueue<int, QueueNode> PriorityQueue;

      public:
        Pathfinder(NavMap& navMap);

        bool FindPath(int2& start, int2& goal, Components::Path& path);

        static void GetPath(int startIndex, int goalIndex, Node* nodeSet, Path& path);

      private:
        static bool FindPath(NavMap&                              navMap,
                             int2&                                start,
                             int2&                                goal,
                             Path&                                path,
                             Core::PriorityQueue<int, QueueNode>& openSet,
                             Node*                                nodeSet);

        static void ProcessDirections(NavMap&                                        navMap,
                                      int2&                                          pos,
                                      int2&                                          goal,
                                      int                                            index,
                                      int                                            parentCost,
                                      Overseer::Core::PriorityQueue<int, QueueNode>& openSet,
                                      Node*                                          nodeSet,
                                      int4x2                                         offsets);
    };
} // namespace Overseer::Core

#endif // OVERSEER_WASM_SRC_CORE_PATHFINDER_H_
