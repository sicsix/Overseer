//
// Created by Tim on 26/12/2021.
//

#ifndef OVERSEER_WASM_SRC_COMMON_MATH_H_
#define OVERSEER_WASM_SRC_COMMON_MATH_H_
#include "Includes.h"

namespace Overseer::Common::Math
{
    static constexpr int4x2 CardinalOffsetsSIMD = int4x2(int4(0, 1, 0, -1), int4(1, 0, -1, 0));
    static constexpr int4x2 DiagonalOffsetsSIMD = int4x2(int4(1, 1, -1, -1), int4(1, -1, -1, 1));

    inline static int2 IndexToPos(int index, int width)
    {
        return int2(index % width, index / width);
    }

    inline static int PosToIndex(int2& pos, int width)
    {
        return pos.y * width + pos.x;
    }

    inline static int4 PosToIndexSIMD(int4x2& positions, int width)
    {
        return int4(positions.y * width + positions.x);
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
        return maxValue - maxValue * linalg::pow((min(distance, maxDistance) / maxDistance), 2);
    }

    inline static float CalculateInverse4PolyInfluence(float maxValue, float distance, float maxDistance)
    {
        return maxValue - maxValue * linalg::pow((min(distance, maxDistance) / maxDistance), 4);
    }
} // namespace Overseer::Common::Math

using namespace Overseer::Common::Math;

#endif // OVERSEER_WASM_SRC_COMMON_MATH_H_
