//
// Created by Tim on 26/12/2021.
//

#ifndef OVERSEER_WASM_SRC_COMPONENTS_COMPONENTS_H_
#define OVERSEER_WASM_SRC_COMPONENTS_COMPONENTS_H_
#include "Includes.h"

namespace Overseer::Components
{
    struct My
    {
    };

    struct Pos
    {
        int2 Val;

        Pos(const int2& val): Val(val)
        {
        }
    };

    struct Health
    {
        int Hits;
        int HitsMax;

        Health(const int& hits, const int& hitsMax): Hits(hits), HitsMax(hitsMax)
        {
        }
    };

    struct Path
    {
        static constexpr int Size = 128;

        int2 Val[Size];
        //		std::array<int2, 128> Val;

        int Count = 0;

        void Add(int2& pos)
        {
            Val[Count++] = pos;
        }

        void Clear()
        {
            Count = 0;
        }
    };

    struct CreepIMAP
    {
        int2 Start;
        int2 End;
        int2 InfCenter;
        int MapStartIndex;
        int InfStartIndex;
        int MapYIncrement;
        int InfYIncrement;
        float* Influence = new float[INFLUENCE_SIZE];
    };

    struct CreepProxIMAP : CreepIMAP
    {
    };

    struct CreepThreatIMAP : CreepIMAP
    {
    };

} // namespace Overseer::Components

using namespace Overseer::Components;
#endif // OVERSEER_WASM_SRC_COMPONENTS_COMPONENTS_H_
