//
// Created by Tim on 26/12/2021.
//

#ifndef OVERSEER_WASM_SRC_CORE_MATH_H_
#define OVERSEER_WASM_SRC_CORE_MATH_H_
#include "Includes.h"

namespace Overseer::Core::Math
{
    static constexpr int4x2 CardinalOffsetsSIMD = int4x2(int4(0, 1, 0, -1), int4(1, 0, -1, 0));
    static constexpr int4x2 DiagonalOffsetsSIMD = int4x2(int4(1, 1, -1, -1), int4(1, -1, -1, 1));

    inline static int2 IndexToPos(int index, int width)
    {
        return int2(index % width, index / width);
    }

    inline static int PosToIndex(int2 pos, int width)
    {
        return pos.y * width + pos.x;
    }

    inline static int4 PosToIndexSIMD(int4x2& positions, int width)
    {
        return int4(positions.y * width + positions.x);
    }

    inline static bool IsPosInbounds(int2& cell, int2 dimensions)
    {
        return ((cell < dimensions) & (cell >= int2(0))) == 1;
    }

    inline static bool IsPosInbounds(int2& cell, int2 start, int2 end)
    {
        return ((cell < end) & (cell >= start)) == 1;
    }

    inline static int4 IsPosInboundsSIMD(int4x2& positions, int2 dimensions)
    {
        // bool4 a = less(positions.x, dimensions.x);
        // bool4 b = gequal(positions.x, 0);
        // bool4 c = less(positions.y, dimensions.y);
        // bool4 d = gequal(positions.y, 0);
        // return (bool4)(a & b & c & d);
        return less(positions.x, dimensions.x) & gequal(positions.x, 0) & less(positions.y, dimensions.y) &
               gequal(positions.y, 0);
    }
    inline static int4 IsPosInboundsSIMD(int4x2& positions, int2& start, int2 end)
    {
        return less(positions.x, end.x) & gequal(positions.x, start.x) & less(positions.y, end.y) &
               gequal(positions.y, start.y);
    }

    inline static int2 WorldToLocal(int2& worldPos, int2& localWorldStart)
    {
        return worldPos - localWorldStart;
    }

    inline static Direction GetDirection(int2& orig, int2& dest)
    {
        int2 vector = dest - orig + 1;
        int vecIndex = vector.y * 3 + vector.x;

        switch (vecIndex)
        {
            case 0:
                return Direction::TOP_LEFT;
            case 1:
                return Direction::TOP;
            case 2:
                return Direction::TOP_RIGHT;
            case 3:
                return Direction::LEFT;
            case 5:
                return Direction::RIGHT;
            case 6:
                return Direction::BOTTOM_LEFT;
            case 7:
                return Direction::BOTTOM;
            case 8:
                return Direction::BOTTOM_RIGHT;
        }
        throw printf("[WASM] ERROR: Invalid direction\n");
    }

    inline static int DistanceManhattan(int2& orig, int2& dest)
    {
        return sum(abs(dest - orig));
    }

    inline static int DistanceChebyshev(int2 orig, int2 dest)
    {
        return maxelem(abs(orig - dest));
    }

    inline static int4 DistanceChebyshevSIMD(int4x2& origins, int2& dest)
    {
        int4x2 absVal = int4x2(abs(origins.x - dest.x), abs(origins.y - dest.y));
        return int4(maxelem(absVal.row(0)), maxelem(absVal.row(1)), maxelem(absVal.row(2)), maxelem(absVal.row(3)));
    }

    inline static float CalculateInverseLinearInfluence(float maxValue, float distance, float maxDistance)
    {
        return maxValue - maxValue * (min(distance, maxDistance) / maxDistance);
    }

    inline static float CalculateInverse2PolyInfluence(float maxValue, float distance, float maxDistance)
    {
        float val = min(distance, maxDistance) / maxDistance;
        return maxValue - maxValue * (val * val);
    }

    inline static float CalculateInverse4PolyInfluence(float maxValue, float distance, float maxDistance)
    {
        float val = min(distance, maxDistance) / maxDistance;
        return maxValue - maxValue * (val * val * val * val);
    }
} // namespace Overseer::Core::Math

using namespace Overseer::Core::Math;

#endif // OVERSEER_WASM_SRC_CORE_MATH_H_
