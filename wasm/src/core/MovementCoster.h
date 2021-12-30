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
        NavMap                   NavigationMap;
        PriorityQueue<int, Node> PriorityQueue;

      public:
        MovementCoster(NavMap& navMap);

        void Update(CreepThreatIMAP& threatIMAP, CreepMovementMap& creepMovementMap);

      private:
        static void Update(NavMap&                         navMap,
                           CreepThreatIMAP&                threatIMAP,
                           CreepMovementMap&               creepMovementMap,
                           Core::PriorityQueue<int, Node>& openSet);

        static void ProcessDirections(NavMap&                         navMap,
                                      int2&                           currWorldPos,
                                      int2&                           currInfPos,
                                      int2&                           worldStart,
                                      int2&                           worldEnd,
                                      int                             parentCost,
                                      CreepMovementMap&               creepMovementMap,
                                      Core::PriorityQueue<int, Node>& openSet,
                                      int4x2                          offsets);
    };
} // namespace Overseer::Core

#endif // OVERSEER_WASM_SRC_CORE_MOVEMENTCOSTER_H_
