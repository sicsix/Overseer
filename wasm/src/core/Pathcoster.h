//
// Created by Tim on 28/12/2021.
//

#ifndef OVERSEER_WASM_SRC_CORE_PATHCOSTER_H_
#define OVERSEER_WASM_SRC_CORE_PATHCOSTER_H_
#include "Includes.h"
#include "Structures.h"
#include "components/Components.h"
#include "PriorityQueue.h"

namespace Overseer::Core
{
    class Pathcoster
    {
      private:
        NavMap                   NavigationMap;
        PriorityQueue<int, Node> PriorityQueue;

      public:
        Pathcoster(NavMap& navMap);

        void GetPathingCosts(CreepThreatIMAP& threatIMAP, int costMultiplier, int* costs);

      private:
        static void GetPathingCosts(NavMap&                           navMap,
                                   CreepThreatIMAP&                  threatIMAP,
                                   int                               costMultiplier,
                                   int*                              costs,
                                    Core::PriorityQueue<int, Node>& openSet);

        static void ProcessDirections(NavMap&                           navMap,
                                      int2&                             currWorldPos,
                                      int2&                             currInfPos,
                                      int2&                             worldStart,
                                      int2&                             worldEnd,
                                      int                               parentCost,
                                      int                               costMultiplier,
                                      int*                              costs,
                                      Core::PriorityQueue<int, Node>& openSet,
                                      int4x2                            offsets);
    };
} // namespace Overseer::Core

#endif // OVERSEER_WASM_SRC_CORE_PATHCOSTER_H_
