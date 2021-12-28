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
        NavMap                   NavigationMap;
        PriorityQueue<int, Node> PriorityQueue;

      public:
        Pathcoster(NavMap& navMap);

        void GetAttackCosts(CreepThreatIMAP& threatIMAP, int costMultiplier, int* costs);

      private:
        static void GetAttackCosts(NavMap&                           navMap,
                                   CreepThreatIMAP&                  threatIMAP,
                                   int                               costMultiplier,
                                   int*                              costs,
                                   Common::PriorityQueue<int, Node>& openSet);

        static void ProcessDirections(NavMap&                           navMap,
                                      int2&                             currWorldPos,
                                      int2&                             worldStart,
                                      int2&                             worldEnd,
                                      int                               parentCost,
                                      int                               costMultiplier,
                                      int*                              costs,
                                      Common::PriorityQueue<int, Node>& openSet,
                                      int4x2                            offsets);
    };
} // namespace Overseer::Common

#endif // OVERSEER_WASM_SRC_COMMON_PATHCOSTER_H_
