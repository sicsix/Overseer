//
// Created by Tim on 30/12/2021.
//

#ifndef OVERSEER_WASM_SRC_SYSTEMS_AI_SQUAD_H_
#define OVERSEER_WASM_SRC_SYSTEMS_AI_SQUAD_H_
#include "Includes.h"
#include "entt.hpp"

namespace Overseer::Systems::AI
{
    class Squad
    {
        int   Size;
        int   TotalHP;
        int   TotalRangedHealing;
        int   TotalAttackDamage;
        int   TotalRangedAttackDamage;
        int   CalculatedThreat;
        float AverageTicksPerMove;

        entt::entity              Leader;
        entt::entity              Grunts[MAX_SQUAD_SIZE];
    };
} // namespace Overseer::Systems::AI
#endif // OVERSEER_WASM_SRC_SYSTEMS_AI_SQUAD_H_
