//
// Created by Tim on 28/12/2021.
//

#include "Pathcoster.h"
#include "PriorityQueue.h"
#include "Math.h"

namespace Overseer::Common
{
    Pathcoster::Pathcoster(NavMap& navMap)
    {
        NavigationMap = navMap;
        PriorityQueue = Overseer::Common::PriorityQueue<int, Node>();
    }

    // Start and End must be in World space
    void Pathcoster::GetCosts(int2& orig, int* costs, int2& start, int2& end, int costMultiplier)
    {
        GetCosts(NavigationMap, orig, costs, start, end, int costMultiplier, PriorityQueue);
        PriorityQueue.Clear();
    }

    void Pathcoster::GetCosts(NavMap&                           navMap,
                              int2&                             orig,
                              int*                              costs,
                              int2&                             start,
                              int2&                             end,
                              int                               costMultiplier,
                              Common::PriorityQueue<int, Node>& openSet)
    {
        memset(costs, INT_MAXVALUE, sizeof(int) * (INFLUENCE_SIZE + 1));

        int startIndex    = PosToIndex(orig, MAP_WIDTH);
        int infStartIndex = PosToIndex(, INFLUENCE_WIDTH);

        for (int y = 0; y < ; ++y)
        {
            for (int x = 0; x < ; ++x)
            {
                int currIndex = ;
                if (navMap[currIndex] != INT_MAXVALUE)
                {
                    costs[infIndex] = 0;

                    if (currIndex != startIndex)
                        openSet.Push(0, Node(currIndex, 0))
                }
            }
        }

        while (!openSet.Empty())
        {
            const Node current = openSet.Pop();

            auto visitedNodeCost = costs[currInfIndex];

            if (visitedNodeCost < current.CostSoFar)
                continue;

            auto pos = IndexToPos(current.Index, MAP_WIDTH);

            ProcessDirections(
                navMap, pos, current.Index, visitedNodeCost, costs, start, end, openSet, CardinalOffsetsSIMD);
            ProcessDirections(
                navMap, pos, current.Index, visitedNodeCost, costs, start, end, openSet, DiagonalOffsetsSIMD);
        }
    }

    void Pathcoster::ProcessDirections(NavMap&                           navMap,
                                       int2&                             pos,
                                       int                               index,
                                       int                               parentCost,
                                       int*                              costs,
                                       int2&                             start,
                                       int2&                             end,
                                       int                               costMultiplier,
                                       Common::PriorityQueue<int, Node>& openSet,
                                       int4x2                            offsets)
    {
        int4x2 currentPositions       = int4x2(int4(pos.x), int4(pos.y)) + offsets;
        int4   isPosInbounds          = IsPosInboundsSIMD(currentPositions, MAP_DIMENSIONS);
        int4   isPosInInfluenceBounds = IsPosInInfluenceBoundsSIMD(currentPositions, start, end);
        int4   currentIndexes         = PosToIndexSIMD(currentPositions, MAP_WIDTH);
        currentIndexes                = clamp(currentIndexes, 0, MAP_SIZE - 1);

        // printf(
        //     "CurrentPostions: {{ %i, %i }, { %i, %i }, { %i, %i } { %i, %i }   IsPosInbounds: { %i, %i, %i, %i }
        //     CurrentIndexes: { %i, %i, %i, %i }\n", currentPositions.x.x, currentPositions.y.x, currentPositions.x.y,
        //     currentPositions.y.y,
        //     currentPositions.x.z,
        //     currentPositions.y.z,
        //     currentPositions.x.w,
        //     currentPositions.y.w,
        //     isPosInbounds.x,
        //     isPosInbounds.y,
        //     isPosInbounds.y,
        //     isPosInbounds.z,
        //     currentIndexes.x,
        //     currentIndexes.y,
        //     currentIndexes.z,
        //     currentIndexes.w);

        int4 costSoFar = int4(navMap.Map[currentIndexes.x],
                              navMap.Map[currentIndexes.y],
                              navMap.Map[currentIndexes.z],
                              navMap.Map[currentIndexes.w]) *
                             costMultiplier +
                         parentCost;

        int4 prevCostSoFar = int4(nodeSet[currentIndexes.x].Cost,
                                  nodeSet[currentIndexes.y].Cost,
                                  nodeSet[currentIndexes.z].Cost,
                                  nodeSet[currentIndexes.w].Cost);

        // printf("CostSoFar: { %i, %i, %i, %i }    PrevCostSoFar { %i, %i, %i, %i }\n",
        //        costSoFar.x,
        //        costSoFar.y,
        //        costSoFar.z,
        //        costSoFar.w,
        //        prevCostSoFar.x,
        //        prevCostSoFar.y,
        //        prevCostSoFar.z,
        //        prevCostSoFar.w);

        bool4 isBetterRoute = less(costSoFar, prevCostSoFar);
        // printf(
        //     "IsBetterRoute { %i, %i, %i, %i }\n", isBetterRoute.x, isBetterRoute.y, isBetterRoute.z,
        //     isBetterRoute.w);

        bool4 isTileValid =
            (bool4)(!gequal(costSoFar, INT_MAXVALUE) & isPosInbounds & isPosInInfluenceBounds & isBetterRoute);
        currentIndexes = select(isTileValid, currentIndexes, MAP_SIZE); // MAP_SIZE is outside map

        nodeSet[currentIndexes.x] = CostNode(costSoFar.x);
        nodeSet[currentIndexes.y] = CostNode(costSoFar.y);
        nodeSet[currentIndexes.z] = CostNode(costSoFar.z);
        nodeSet[currentIndexes.w] = CostNode(costSoFar.w);

        // printf("IsTileValid { %i, %i, %i, %i }\n", isTileValid.x, isTileValid.y, isTileValid.z, isTileValid.w);
        // printf(
        //     "ExpectedCosts { %i, %i, %i, %i }\n", expectedCosts.x, expectedCosts.y, expectedCosts.z,
        //     expectedCosts.w);

        if (isTileValid.x)
            openSet.Push(costSoFar.x, Node(currentIndexes.x, costSoFar.x));
        if (isTileValid.y)
            openSet.Push(costSoFar.y, Node(currentIndexes.y, costSoFar.y));
        if (isTileValid.z)
            openSet.Push(costSoFar.z, Node(currentIndexes.z, costSoFar.z));
        if (isTileValid.w)
            openSet.Push(costSoFar.w, Node(currentIndexes.w, costSoFar.w));
    }
} // namespace Overseer::Common
