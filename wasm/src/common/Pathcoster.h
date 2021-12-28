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
        PriorityQueue<int, Node> PriorityQueue;

      public:
        Pathcoster(NavMap& navMap);

        void GetCosts(int2& orig, int* costs, int2& start, int2& end, int costMultiplier);

      private:
        static void GetCosts(NavMap&                           navMap,
                             int2&                             orig,
                             int*                              costs,
                             int2&                             start,
                             int2&                             end,
                             int                               costMultiplier,
                             Common::PriorityQueue<int, Node>& openSet);

        static void ProcessDirections(NavMap&                           navMap,
                                      int2&                             pos,
                                      int                               index,
                                      int                               parentCost,
                                      int*                              costs,
                                      int2&                             start,
                                      int2&                             end,
                                      int                               costMultiplier,
                                      Common::PriorityQueue<int, Node>& openSet,
                                      int4x2                            offsets)
    };
} // namespace Overseer::Common

#endif // OVERSEER_WASM_SRC_COMMON_PATHCOSTER_H_
