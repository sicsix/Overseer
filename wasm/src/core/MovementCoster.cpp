//
// Created by Tim on 28/12/2021.
//

#include "MovementCoster.h"
#include "PriorityQueue.h"
#include "Math.h"

namespace Overseer::Core
{
    MovementCoster::MovementCoster(NavMap& navMap)
    {
        NavigationMap = navMap;
        BlankNodeSet  = new Node[INTEREST_SIZE + 1];
        PriorityQueue = Overseer::Core::PriorityQueue<int, QueueNode>();
    }

    void MovementCoster::Update(CreepMovementMap& creepMovementMap)
    {
        memcpy(creepMovementMap.Nodes, BlankNodeSet, sizeof(Node) * (INTEREST_SIZE + 1));
        Update(NavigationMap, creepMovementMap, PriorityQueue);
        PriorityQueue.Clear();
    }

    void MovementCoster::Update(NavMap&                              navMap,
                                CreepMovementMap&                    movementMap,
                                Core::PriorityQueue<int, QueueNode>& openSet)
    {
        // printf(
        //     "WorldStart: { %i, %i }    WorldEnd: { %i, %i }    WorldCenter: { %i, %i }    WorldStartIndex: %i
        //     MapYIncrement: %i    InfStart: { %i, %i }    InfEnd: { %i, %i }    InfStartIndex: %i    InfYIncrement:
        //     %i\n", threatIMAP.WorldStart.x, threatIMAP.WorldStart.y, threatIMAP.WorldEnd.x, threatIMAP.WorldEnd.y,
        //     threatIMAP.WorldCenter.x,
        //     threatIMAP.WorldCenter.y,
        //     threatIMAP.WorldStartIndex,
        //     threatIMAP.MapYIncrement,
        //     threatIMAP.InfStart.x,
        //     threatIMAP.InfStart.y,
        //     threatIMAP.InfEnd.x,
        //     threatIMAP.InfEnd.y,
        //     threatIMAP.InfStartIndex,
        //     threatIMAP.InfYIncrement);

        int centerIndex = PosToIndex(movementMap.WorldCenter, MAP_WIDTH);

        int2 centerStartUnclamped = movementMap.WorldCenter - 1;
        int2 centerStart          = max(centerStartUnclamped, 0);
        int2 centerEnd            = min(movementMap.WorldCenter + 2, MAP_WIDTH);

        int2 offset   = centerStart - centerStartUnclamped;
        int2 infStart = INTEREST_CENTER - 1 + offset;

        int width         = centerEnd.x - centerStart.x;
        int mapYIncrement = MAP_WIDTH - width;
        int infYIncrement = INTEREST_WIDTH - width;

        int mapIndex = PosToIndex(centerStart, MAP_WIDTH);
        int infIndex = PosToIndex(infStart, INTEREST_WIDTH);

        // printf(
        //     "WorldCenter: { %i, %i }    WorldStart: { %i, %i }    WorldEnd: { %i, %i }   WorldStartIndex: % i    LocalCenter: { %i, %i }    LocalStart: { %i, %i }    LocalEnd: { %i, %i }   LocalStartIndex: % i\n",
        //     movementMap.WorldCenter.x,
        //     movementMap.WorldCenter.y,
        //     movementMap.WorldStart.x,
        //     movementMap.WorldStart.y,
        //     movementMap.WorldEnd.x,
        //     movementMap.WorldEnd.y,
        //     movementMap.WorldStartIndex,
        //     movementMap.LocalCenter.x,
        //     movementMap.LocalCenter.y,
        //     movementMap.LocalStart.x,
        //     movementMap.LocalStart.y,
        //     movementMap.LocalEnd.x,
        //     movementMap.LocalEnd.y,
        //     movementMap.LocalStartIndex);

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

        // TODO this is a little clumsy given the repurposing of movementcoster, this could probably be better managed in the threat calculations by checking dist == minDist
        // TODO original purpose of this code was to make sure the threat level adjacent to a creep was the maximum value
        for (int y = centerStart.y; y < centerEnd.y; ++y)
        {
            for (int x = centerStart.x; x < centerEnd.x; ++x)
            {
                if (navMap.Map[mapIndex] != INT_MAXVALUE)
                {
                    movementMap.Nodes[infIndex] = Node(centerIndex, 0);

                    if (mapIndex != centerIndex)
                        openSet.Push(0, QueueNode(mapIndex, 0));
                }
                mapIndex++;
                infIndex++;
            }
            mapIndex += mapYIncrement;
            infIndex += infYIncrement;
        }

        while (!openSet.Empty())
        {
            const QueueNode current = openSet.Pop();

            int2 currWorldPos = IndexToPos(current.Index, MAP_WIDTH);
            int2 currInfPos   = currWorldPos - (movementMap.WorldCenter - INTEREST_RADIUS);
            int  currInfIndex = PosToIndex(currInfPos, INTEREST_WIDTH);

            auto visitedNode = movementMap.Nodes[currInfIndex];

            if (visitedNode.Cost < current.CostSoFar)
                continue;

            // printf(
            //     "    CurrWorldPos: { %i, %i}    CurrInfPos: { %i, %i }    WorldIndex: %i    InfIndex: %i    CostSoFar: %i    NodeCost: %i\n",
            //     currWorldPos.x,
            //     currWorldPos.y,
            //     currInfPos.x,
            //     currInfPos.y,
            //     current.Index,
            //     currInfIndex,
            //     current.CostSoFar,
            //     visitedNode.Cost);

            ProcessDirections(navMap,
                              currWorldPos,
                              currInfPos,
                              current.Index,
                              visitedNode.Cost,
                              movementMap,
                              openSet,
                              CardinalOffsetsSIMD);
            ProcessDirections(navMap,
                              currWorldPos,
                              currInfPos,
                              current.Index,
                              visitedNode.Cost,
                              movementMap,
                              openSet,
                              DiagonalOffsetsSIMD);
        }
    }

    void MovementCoster::ProcessDirections(NavMap&                              navMap,
                                           int2&                                currWorldPos,
                                           int2&                                currInfPos,
                                           int                                  index,
                                           int                                  parentCost,
                                           CreepMovementMap&                    creepMovementMap,
                                           Core::PriorityQueue<int, QueueNode>& openSet,
                                           int4x2                               offsets)
    {
        int4x2 currentWorldPositions = int4x2(int4(currWorldPos.x), int4(currWorldPos.y)) + offsets;
        int4   isPosInbounds =
            IsPosInboundsSIMD(currentWorldPositions, creepMovementMap.WorldStart, creepMovementMap.WorldEnd);
        int4 currentWorldIndexes = PosToIndexSIMD(currentWorldPositions, MAP_WIDTH);
        currentWorldIndexes      = clamp(currentWorldIndexes, 0, MAP_SIZE - 1);

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

        int4 costSoFar = tileCosts + parentCost;

        // printf("        TileCosts: { %i, %i, %i, %i }\n", tileCosts.x, tileCosts.y, tileCosts.z, tileCosts.w);

        int4x2 currentInfPositions = int4x2(int4(currInfPos.x), int4(currInfPos.y)) + offsets;
        int4   currentInfIndexes   = PosToIndexSIMD(currentInfPositions, INTEREST_WIDTH);

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

        int4 prevCostSoFar = int4(creepMovementMap.Nodes[currentInfIndexes.x].Cost,
                                  creepMovementMap.Nodes[currentInfIndexes.y].Cost,
                                  creepMovementMap.Nodes[currentInfIndexes.z].Cost,
                                  creepMovementMap.Nodes[currentInfIndexes.w].Cost);

        bool4 isBetterRoute = less(costSoFar, prevCostSoFar);
        // printf(
        //     "        IsBetterRoute: { %i, %i, %i, %i }\n", isBetterRoute.x, isBetterRoute.y, isBetterRoute.z,
        //     isBetterRoute.w);

        bool4 isTileValid = (bool4)(!equal(tileCosts, INT_MAXVALUE) & isPosInbounds & isBetterRoute);
        currentInfIndexes =
            select(isTileValid, currentInfIndexes, INTEREST_SIZE); // INTEREST_SIZE is outside influence map

        // printf("        IsTileValid: { %i, %i, %i, %i }\n", isTileValid.x, isTileValid.y, isTileValid.z,
        // isTileValid.w); printf("        CostSoFar: { %i, %i, %i, %i }\n", costSoFar.x, costSoFar.y, costSoFar.z,
        // costSoFar.w);

        creepMovementMap.Nodes[currentInfIndexes.x] = Node(index, costSoFar.x);
        creepMovementMap.Nodes[currentInfIndexes.y] = Node(index, costSoFar.y);
        creepMovementMap.Nodes[currentInfIndexes.z] = Node(index, costSoFar.z);
        creepMovementMap.Nodes[currentInfIndexes.w] = Node(index, costSoFar.w);

        if (isTileValid.x)
            openSet.Push(costSoFar.x, QueueNode(currentWorldIndexes.x, costSoFar.x));
        if (isTileValid.y)
            openSet.Push(costSoFar.y, QueueNode(currentWorldIndexes.y, costSoFar.y));
        if (isTileValid.z)
            openSet.Push(costSoFar.z, QueueNode(currentWorldIndexes.z, costSoFar.z));
        if (isTileValid.w)
            openSet.Push(costSoFar.w, QueueNode(currentWorldIndexes.w, costSoFar.w));
    }

    void MovementCoster::GetPath(int startIndex, int goalIndex, CreepMovementMap& movementMap, Path& path)
    {
        // int worldIndex = movementMap.WorldStartIndex;
        // int localIndex = movementMap.LocalStartIndex;
        //
        // int width           = movementMap.LocalEnd.x - movementMap.LocalStart.x;
        // int worldYIncrement = MAP_WIDTH - width;
        // int localYIncrement = INTEREST_WIDTH - width;
        //
        // printf("CosterGetPath: { ");
        // for (int y = movementMap.LocalStart.y; y < movementMap.LocalEnd.y; ++y)
        // {
        //     for (int x = movementMap.LocalStart.x; x < movementMap.LocalEnd.x; ++x)
        //     {
        //         int  cameFrom    = movementMap.Nodes[localIndex].CameFrom;
        //         int2 cameFromPos = IndexToPos(cameFrom, MAP_WIDTH);
        //         printf("{ WorldIndex: %i, LocalIndex: %i, CameFromIndex: %i, CameFromPos: { %i, %i } }, ",
        //                worldIndex,
        //                localIndex,
        //                cameFrom,
        //                cameFromPos.x,
        //                cameFromPos.y);
        //         worldIndex++;
        //         localIndex++;
        //     }
        //     worldIndex += worldYIncrement;
        //     localIndex += localYIncrement;
        // }
        // printf("} \n");

        // return;

        int  currentIndex    = goalIndex;
        int2 currentWorldPos = IndexToPos(currentIndex, MAP_WIDTH);

        int reversePathArray[128]; // TODO pass this in and reuse it
        int pathIndex = 0;

        do
        {
            // TODO optimise this by just determine direction of movement rather than recalculating?
            reversePathArray[pathIndex++] = currentIndex;
            currentWorldPos               = IndexToPos(currentIndex, MAP_WIDTH);
            int currentLocalIndex = PosToIndex(WorldToLocal(currentWorldPos, movementMap.WorldStart), INTEREST_WIDTH) +
                                    movementMap.LocalStartIndex;
            auto node = movementMap.Nodes[currentLocalIndex];
            // printf("Path: %i    WorldIndex: %i    CameFrom: %i    CurrentLocalIndex: %i\n",
            //        pathIndex - 1,
            //        currentIndex,
            //        node.CameFrom,
            //        currentLocalIndex);
            currentIndex = node.CameFrom;
            if (pathIndex >= 128)
                throw printf("[WASM] Path length exceeded maximum of 128\n");
        } while (currentIndex != startIndex);

        int currIndex = 0;
        for (int i = pathIndex - 1; i >= 0; --i)
        {
            path.Val[currIndex] = IndexToPos(reversePathArray[i], MAP_WIDTH);
            currIndex++;
        }
        path.Count = pathIndex;
    }
} // namespace Overseer::Core
