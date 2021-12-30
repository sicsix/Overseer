//
// Created by Tim on 30/12/2021.
//

#ifndef OVERSEER_WASM_SRC_SYSTEMS_AI_AISYSTEM_H_
#define OVERSEER_WASM_SRC_SYSTEMS_AI_AISYSTEM_H_
#include "systems/SystemBase.h"
#include "components/Components.h"
#include "core/Influence.h"
#include "commands/CommandHandler.h"
#include "core/Math.h"
#include "core/MovementCoster.h"

namespace Overseer::Systems::AI
{
    class AISystem : SystemBase
    {
        void Initialise(entt::registry& registry) override
        {

        }

        void Update(entt::registry& registry) override
        {

        }
    };
}
#endif // OVERSEER_WASM_SRC_SYSTEMS_AI_AISYSTEM_H_
