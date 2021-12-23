//
// Created by Tim on 28/12/2021.
//

#include "Pathcoster.h"
#include "PriorityQueue.h"
#include "Math.h"

namespace Overseer::Common
{
    struct Pathcoster::CostNode
    {
        int Cost = 2147483647;

        CostNode()
        {
        }

        CostNode(int cost): Cost(cost)
        {
        }
    };

    Pathcoster::Pathcoster(NavMap& navMap)
    {
        NavigationMap = navMap;
        BlankNodeSet  = new CostNode[MAP_SIZE + 1];
        NodeSet       = new CostNode[MAP_SIZE + 1];
        PriorityQueue = Overseer::Common::PriorityQueue<int, Node>();
    }
} // namespace Overseer::Common
