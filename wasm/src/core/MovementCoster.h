//
// Created by Tim on 28/12/2021.
//

#ifndef OVERSEER_WASM_SRC_CORE_MOVEMENTCOSTER_H_
#define OVERSEER_WASM_SRC_CORE_MOVEMENTCOSTER_H_
#include "Includes.h"
#include "Structures.h"
#include "components/Components.h"
#include "PriorityQueue.h"
#include "core/Influence/CreepMovement.h"

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

        void Update(Influence::CreepMovement& movement);

        static void GetPath(int startIndex, int goalIndex, Influence::CreepMovement& movement, Path& path);

      private:
        static void Update(NavMap&                              navMap,
                           Influence::CreepMovement&            movement,
                           Core::PriorityQueue<int, QueueNode>& openSet);

        static void ProcessDirections(NavMap&                              navMap,
                                      int2&                                currWorldPos,
                                      int2&                                currInfPos,
                                      int                                  index,
                                      int                                  parentCost,
                                      Influence::CreepMovement&            movement,
                                      Core::PriorityQueue<int, QueueNode>& openSet,
                                      int4x2                               offsets);
    };
} // namespace Overseer::Core

#endif // OVERSEER_WASM_SRC_CORE_MOVEMENTCOSTER_H_
