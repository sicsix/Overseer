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

    void Pathcoster::GetAttackCosts(CreepThreatIMAP& threatIMAP, int costMultiplier, int* costs)
    {
        GetAttackCosts(NavigationMap, threatIMAP, costMultiplier, costs, PriorityQueue);
        PriorityQueue.Clear();
    }

    // TODO rename GetAttackCosts
    void Pathcoster::GetAttackCosts(NavMap&                           navMap,
                                    CreepThreatIMAP&                  threatIMAP,
                                    int                               costMultiplier,
                                    int*                              costs,
                                    Common::PriorityQueue<int, Node>& openSet)
    {
        for (int i = 0; i <= INFLUENCE_SIZE; ++i) // <= as we the size is inf size + 1
        {
            costs[i] = INT_MAXVALUE;
        }

        // printf(
        //     "WorldStart: { %i, %i }    WorldEnd: { %i, %i }    WorldCenter: { %i, %i }    MapStartIndex: %i    MapYIncrement: %i    InfStart: { %i, %i }    InfEnd: { %i, %i }    InfStartIndex: %i    InfYIncrement: %i\n",
        //     threatIMAP.WorldStart.x,
        //     threatIMAP.WorldStart.y,
        //     threatIMAP.WorldEnd.x,
        //     threatIMAP.WorldEnd.y,
        //     threatIMAP.WorldCenter.x,
        //     threatIMAP.WorldCenter.y,
        //     threatIMAP.MapStartIndex,
        //     threatIMAP.MapYIncrement,
        //     threatIMAP.InfStart.x,
        //     threatIMAP.InfStart.y,
        //     threatIMAP.InfEnd.x,
        //     threatIMAP.InfEnd.y,
        //     threatIMAP.InfStartIndex,
        //     threatIMAP.InfYIncrement);

        int centerIndex = PosToIndex(threatIMAP.WorldCenter, MAP_WIDTH);

        int2 centerStartUnclamped = threatIMAP.WorldCenter - 1;
        int2 centerStart          = max(centerStartUnclamped, 0);
        int2 centerEnd            = min(threatIMAP.WorldCenter + 2, MAP_WIDTH);

        int2 offset   = centerStart - centerStartUnclamped;
        int2 infStart = INFLUENCE_CENTER - 1 + offset;

        int width         = centerEnd.x - centerStart.x;
        int mapYIncrement = MAP_WIDTH - width;
        int infYIncrement = INFLUENCE_WIDTH - width;

        int mapIndex = PosToIndex(centerStart, MAP_WIDTH);
        int infIndex = PosToIndex(infStart, INFLUENCE_WIDTH);

        // printf("Offset: { %i, %i }\n", offset.x, offset.y);
        // printf(
        //     "CenterIndex: %i    CenterStart: { %i, %i }    CenterEnd: { %i, %i }    InfStart: { %i, %i }    Width: %i    MapYIncrement: %i    InfYIncrement: %i    MapIndex: %i    InfIndex: %i\n",
        //     centerIndex,
        //     centerStart.x,
        //     centerStart.y,
        //     centerEnd.x,
        //     centerEnd.y,
        //     infStart.x,
        //     infStart.y,
        //     width,
        //     mapYIncrement,
        //     infYIncrement,
        //     mapIndex,
        //     infIndex);

        for (int y = centerStart.y; y < centerEnd.y; ++y)
        {
            for (int x = centerStart.x; x < centerEnd.x; ++x)
            {
                if (navMap.Map[mapIndex] != INT_MAXVALUE)
                {
                    costs[infIndex] = 0;

                    if (mapIndex != centerIndex)
                        openSet.Push(0, Node(mapIndex, 0));
                }
                mapIndex++;
                infIndex++;
            }
            mapIndex += mapYIncrement;
            infIndex += infYIncrement;
        }

        while (!openSet.Empty())
        {
            const Node current = openSet.Pop();

            int2 currWorldPos = IndexToPos(current.Index, MAP_WIDTH);
            int2 currInfPos   = currWorldPos - (threatIMAP.WorldCenter - INFLUENCE_RADIUS);
            int  currInfIndex = PosToIndex(currInfPos, INFLUENCE_WIDTH);

            auto visitedNodeCost = costs[currInfIndex];

            if (visitedNodeCost < current.CostSoFar)
                continue;

            // printf("    CurrWorldPos: { %i, %i}    CurrInfPos: { %i, %i }    WorldIndex: %i    InfIndex: %i    CostSoFar: %i    NodeCost: %i\n",
            //        currWorldPos.x,
            //        currWorldPos.y,
            //        currInfPos.x,
            //        currInfPos.y,
            //        current.Index,
            //        currInfIndex,
            //        current.CostSoFar,
            //        visitedNodeCost);

            ProcessDirections(navMap,
                              currWorldPos,
                              currInfPos,
                              threatIMAP.WorldStart,
                              threatIMAP.WorldEnd,
                              visitedNodeCost,
                              costMultiplier,
                              costs,
                              openSet,
                              CardinalOffsetsSIMD);
            ProcessDirections(navMap,
                              currWorldPos,
                              currInfPos,
                              threatIMAP.WorldStart,
                              threatIMAP.WorldEnd,
                              visitedNodeCost,
                              costMultiplier,
                              costs,
                              openSet,
                              DiagonalOffsetsSIMD);
        }
    }

    void Pathcoster::ProcessDirections(NavMap&                           navMap,
                                       int2&                             currWorldPos,
                                       int2&                             currInfPos,
                                       int2&                             worldStart,
                                       int2&                             worldEnd,
                                       int                               parentCost,
                                       int                               costMultiplier,
                                       int*                              costs,
                                       Common::PriorityQueue<int, Node>& openSet,
                                       int4x2                            offsets)
    {
        int4x2 currentWorldPositions = int4x2(int4(currWorldPos.x), int4(currWorldPos.y)) + offsets;
        int4   isPosInbounds         = IsPosInboundsSIMD(currentWorldPositions, worldStart, worldEnd);
        int4   currentWorldIndexes   = PosToIndexSIMD(currentWorldPositions, MAP_WIDTH);
        currentWorldIndexes          = clamp(currentWorldIndexes, 0, MAP_SIZE - 1);

        // printf(
        //     "        CurrentWorldPostions: {{ %i, %i }, { %i, %i }, { %i, %i } { %i, %i }   IsPosInbounds: { %i, %i,
        //     %i, %i }    CurrentWorldIndexes: { %i, %i, %i, %i }\n", currentWorldPositions.x.x,
        //     currentWorldPositions.y.x,
        //     currentWorldPositions.x.y,
        //     currentWorldPositions.y.y,
        //     currentWorldPositions.x.z,
        //     currentWorldPositions.y.z,
        //     currentWorldPositions.x.w,
        //     currentWorldPositions.y.w,
        //     isPosInbounds.x,
        //     isPosInbounds.y,
        //     isPosInbounds.y,
        //     isPosInbounds.z,
        //     currentWorldIndexes.x,
        //     currentWorldIndexes.y,
        //     currentWorldIndexes.z,
        //     currentWorldIndexes.w);

        int4 tileCosts = int4(navMap.Map[currentWorldIndexes.x],
                              navMap.Map[currentWorldIndexes.y],
                              navMap.Map[currentWorldIndexes.z],
                              navMap.Map[currentWorldIndexes.w]);

        int4 costSoFar = tileCosts * costMultiplier + parentCost;

        // printf("        TileCosts: { %i, %i, %i, %i }\n", tileCosts.x, tileCosts.y, tileCosts.z, tileCosts.w);

        int4x2 currentInfPositions = int4x2(int4(currInfPos.x), int4(currInfPos.y)) + offsets;
        int4   currentInfIndexes   = PosToIndexSIMD(currentInfPositions, INFLUENCE_WIDTH);

        // printf(
        //     "        CurrentInfPositions: {{ %i, %i }, { %i, %i }, { %i, %i } { %i, %i }    CurrentInfIndexes: { %i,
        //     %i, %i, %i }\n", currentInfPositions.x.x, currentInfPositions.y.x, currentInfPositions.x.y,
        //     currentInfPositions.y.y,
        //     currentInfPositions.x.z,
        //     currentInfPositions.y.z,
        //     currentInfPositions.x.w,
        //     currentInfPositions.y.w,
        //     currentInfIndexes.x,
        //     currentInfIndexes.y,
        //     currentInfIndexes.z,
        //     currentInfIndexes.w);

        int4 prevCostSoFar = int4(costs[currentInfIndexes.x],
                                  costs[currentInfIndexes.y],
                                  costs[currentInfIndexes.z],
                                  costs[currentInfIndexes.w]);

        bool4 isBetterRoute = less(costSoFar, prevCostSoFar);
        // printf(
        //     "        IsBetterRoute: { %i, %i, %i, %i }\n", isBetterRoute.x, isBetterRoute.y, isBetterRoute.z,
        //     isBetterRoute.w);

        bool4 isTileValid = (bool4)(!equal(tileCosts, INT_MAXVALUE) & isPosInbounds & isBetterRoute);
        currentInfIndexes =
            select(isTileValid, currentInfIndexes, INFLUENCE_SIZE); // INFLUENCE_SIZE is outside influence map

        // printf("        IsTileValid: { %i, %i, %i, %i }\n", isTileValid.x, isTileValid.y, isTileValid.z,
        // isTileValid.w); printf("        CostSoFar: { %i, %i, %i, %i }\n", costSoFar.x, costSoFar.y, costSoFar.z,
        // costSoFar.w);

        costs[currentInfIndexes.x] = costSoFar.x;
        costs[currentInfIndexes.y] = costSoFar.y;
        costs[currentInfIndexes.z] = costSoFar.z;
        costs[currentInfIndexes.w] = costSoFar.w;

        if (isTileValid.x)
            openSet.Push(costSoFar.x, Node(currentWorldIndexes.x, costSoFar.x));
        if (isTileValid.y)
            openSet.Push(costSoFar.y, Node(currentWorldIndexes.y, costSoFar.y));
        if (isTileValid.z)
            openSet.Push(costSoFar.z, Node(currentWorldIndexes.z, costSoFar.z));
        if (isTileValid.w)
            openSet.Push(costSoFar.w, Node(currentWorldIndexes.w, costSoFar.w));
    }
} // namespace Overseer::Common
