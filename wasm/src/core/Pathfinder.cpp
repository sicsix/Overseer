//
// Created by Tim on 26/12/2021.
//

#include "Pathfinder.h"
#include "PriorityQueue.h"
#include "Math.h"

namespace Overseer::Core
{
    Pathfinder::Pathfinder(NavMap& navMap)
    {
        NavigationMap = navMap;
        BlankNodeSet  = new Node[MAP_SIZE + 1];
        NodeSet       = new Node[MAP_SIZE + 1];
        PriorityQueue = Overseer::Core::PriorityQueue<int, QueueNode>();
    }

    bool Pathfinder::FindPath(int2& start, int2& goal, Path& path)
    {
        bool foundPath = FindPath(NavigationMap, start, goal, path, PriorityQueue, NodeSet);
        PriorityQueue.Clear();
        memcpy(NodeSet, BlankNodeSet, sizeof(Node) * (MAP_SIZE + 1));
        return foundPath;
    }

    bool Pathfinder::FindPath(NavMap&                              navMap,
                              int2&                                start,
                              int2&                                goal,
                              Path&                                path,
                              Core::PriorityQueue<int, QueueNode>& openSet,
                              Node*                                nodeSet)
    {
        path.Clear();

        int startIndex = PosToIndex(start, MAP_WIDTH);
        int goalIndex  = PosToIndex(goal, MAP_WIDTH);

        // printf("InfStart Index: %i  - Goal Index: %i\n", startIndex, goalIndex);

        if (startIndex < 0 || startIndex >= MAP_SIZE || goalIndex < 0 || goalIndex >= MAP_SIZE)
            return false;

        // TODO Check start and finish tiles are passable! (not 255)

        // Special case when the start is the same point as the goal
        if (startIndex == goalIndex)
            return true;

        nodeSet[startIndex] = Node(-1, 0);
        openSet.Push(DistanceChebyshev(start, goal), QueueNode(startIndex, 0));

        bool foundPath = false;

        while (!openSet.Empty())
        {
            const QueueNode current = openSet.Pop();

            if (current.Index == goalIndex)
            {
                foundPath = true;
                break;
            }

            auto visitedNode = nodeSet[current.Index];

            if (visitedNode.Cost < current.CostSoFar)
                continue;

            auto pos = IndexToPos(current.Index, MAP_WIDTH); // Maybe store this in openSet?

            // printf("Index: %i   CostSoFar: %i    NodeCost: %i    NodeCameFrom: %i    Pos: { %i, %i}\n",
            //        current.Index,
            //        current.CostSoFar,
            //        visitedNode.Cost,
            //        visitedNode.CameFrom,
            //        pos.x,
            //        pos.y);

            ProcessDirections(
                navMap, pos, goal, current.Index, visitedNode.Cost, openSet, nodeSet, CardinalOffsetsSIMD);
            ProcessDirections(
                navMap, pos, goal, current.Index, visitedNode.Cost, openSet, nodeSet, DiagonalOffsetsSIMD);
        }

        if (!foundPath)
            return false;

        GetPath(startIndex, goalIndex, nodeSet, path);

        return true;
    }

    void Pathfinder::ProcessDirections(NavMap&                              navMap,
                                       int2&                                pos,
                                       int2&                                goal,
                                       int                                  index,
                                       int                                  parentCost,
                                       Core::PriorityQueue<int, QueueNode>& openSet,
                                       Node*                                nodeSet,
                                       int4x2                               offsets)
    {
        int4x2 currentPositions = int4x2(int4(pos.x), int4(pos.y)) + offsets;
        int4   isPosInbounds    = IsPosInboundsSIMD(currentPositions, MAP_DIMENSIONS);
        int4   currentIndexes   = PosToIndexSIMD(currentPositions, MAP_WIDTH);
        currentIndexes          = clamp(currentIndexes, 0, MAP_SIZE - 1);

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

        int4 tileCosts = int4(navMap.Map[currentIndexes.x],
                              navMap.Map[currentIndexes.y],
                              navMap.Map[currentIndexes.z],
                              navMap.Map[currentIndexes.w]);

        int4 costSoFar = tileCosts + parentCost;

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

        int4 heuristics = DistanceChebyshevSIMD(currentPositions, goal);
        // printf("Heuristics { %i, %i, %i, %i }\n", heuristics.x, heuristics.y, heuristics.z, heuristics.w);

        int4  expectedCosts = costSoFar + heuristics;
        bool4 isTileValid   = (bool4)(!equal(tileCosts, INT_MAXVALUE) & isPosInbounds & isBetterRoute);
        currentIndexes      = select(isTileValid, currentIndexes, MAP_SIZE); // MAP_SIZE is outside map

        nodeSet[currentIndexes.x] = Node(index, costSoFar.x);
        nodeSet[currentIndexes.y] = Node(index, costSoFar.y);
        nodeSet[currentIndexes.z] = Node(index, costSoFar.z);
        nodeSet[currentIndexes.w] = Node(index, costSoFar.w);

        // printf("IsTileValid { %i, %i, %i, %i }\n", isTileValid.x, isTileValid.y, isTileValid.z, isTileValid.w);
        // printf(
        //     "ExpectedCosts { %i, %i, %i, %i }\n", expectedCosts.x, expectedCosts.y, expectedCosts.z,
        //     expectedCosts.w);

        if (isTileValid.x)
            openSet.Push(expectedCosts.x, QueueNode(currentIndexes.x, costSoFar.x));
        if (isTileValid.y)
            openSet.Push(expectedCosts.y, QueueNode(currentIndexes.y, costSoFar.y));
        if (isTileValid.z)
            openSet.Push(expectedCosts.z, QueueNode(currentIndexes.z, costSoFar.z));
        if (isTileValid.w)
            openSet.Push(expectedCosts.w, QueueNode(currentIndexes.w, costSoFar.w));
    }

    void Pathfinder::GetPath(int startIndex, int goalIndex, Node* nodeSet, Path& path)
    {
        int currentIndex = goalIndex;
        int reversePathArray[128]; // TODO pass this in and reuse it
        int pathIndex = 0;

        do
        {
            reversePathArray[pathIndex++] = currentIndex;
            auto node                     = nodeSet[currentIndex];
            // printf("Path: %i    WorldIndex: %i    CameFrom: %i\n", pathIndex, currentIndex, node.CameFrom);
            currentIndex                  = node.CameFrom;
#ifdef DEBUG_ENABLED
            if (pathIndex >= 128)
                printf("[WASM] ERROR: Core::Pathfindiner - Path length exceeded maximum of 128\n");
#endif
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