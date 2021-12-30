//
// Created by Tim on 28/12/2021.
//

#ifndef OVERSEER_WASM_SRC_CORE_MOVEMENTCOSTER_H_
#define OVERSEER_WASM_SRC_CORE_MOVEMENTCOSTER_H_
#include "Includes.h"
#include "Structures.h"
#include "components/Components.h"
#include "PriorityQueue.h"

namespace Overseer::Core
{
    class MovementCoster
    {
      private:
        NavMap                        NavigationMap;
        Node*                         BlankNodeSet;
        PriorityQueue<int, QueueNode> PriorityQueue;

      public:
        MovementCoster(NavMap& navMap);

        void Update(CreepMovementMap& creepMovementMap);

        static void GetPath(int startIndex, int goalIndex, CreepMovementMap& movementMap, Path& path);

      private:
        static void Update(NavMap&                              navMap,
                           CreepMovementMap& movementMap,
                           Core::PriorityQueue<int, QueueNode>& openSet);

        static void ProcessDirections(NavMap&                              navMap,
                                      int2&                                currWorldPos,
                                      int2&                                currInfPos,
                                      int                                  index,
                                      int                                  parentCost,
                                      CreepMovementMap&                    creepMovementMap,
                                      Core::PriorityQueue<int, QueueNode>& openSet,
                                      int4x2                               offsets);
    };
} // namespace Overseer::Core

#endif // OVERSEER_WASM_SRC_CORE_MOVEMENTCOSTER_H_
