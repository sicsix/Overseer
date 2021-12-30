//
// Created by Tim on 30/12/2021.
//

#ifndef OVERSEER_WASM_SRC_SYSTEMS_AI_GROUP_H_
#define OVERSEER_WASM_SRC_SYSTEMS_AI_GROUP_H_
#include "Includes.h"
#include "entt.hpp"

namespace Overseer::Systems::AI
{
    class Group
    {
        int   Size;
        int   TotalHP;
        int   TotalRangedHealing;
        int   TotalAttackDamage;
        int   TotalRangedAttackDamage;
        int   CalculatedThreat;
        float AverageTicksPerMove;

        entt::entity              SquadLeader;
        std::vector<entt::entity> Grunts;
    };
} // namespace Overseer::Systems::AI
#endif // OVERSEER_WASM_SRC_SYSTEMS_AI_GROUP_H_
