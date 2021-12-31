//
// Created by Tim on 1/01/2022.
//

#ifndef OVERSEER_WASM_SRC_CORE_UTILITY_H_
#define OVERSEER_WASM_SRC_CORE_UTILITY_H_
#include "components/Components.h"

namespace Overseer::Core
{
    struct Utility
    {
        inline static int CalculateThreatLevel(Threat& threat)
        {
            // TODO improve this, include HP etc
            return threat.AttackDamage + threat.RangedAttackDamage + threat.HealRate;
        }
    };
}
#endif //OVERSEER_WASM_SRC_CORE_UTILITY_H_
