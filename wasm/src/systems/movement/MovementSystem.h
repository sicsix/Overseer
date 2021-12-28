//
// Created by Tim on 26/12/2021.
//

#ifndef OVERSEER_WASM_SRC_SYSTEMS_MOVEMENT_MOVEMENTSYSTEM_H_
#define OVERSEER_WASM_SRC_SYSTEMS_MOVEMENT_MOVEMENTSYSTEM_H_
#include "systems/SystemBase.h"
#include "commands/CommandHandler.h"
#include "components/Components.h"

namespace Overseer::Systems
{
    class MovementSystem : SystemBase
    {
      public:
        void Initialise(entt::registry& registry) override
        {
        }

        void Update(entt::registry& registry) override
        {
            auto view = registry.view<My, Path>();

            for (auto entity : view)
            {
                CommandHandler::Add(Move((double)entity, Direction::TOP_LEFT));
            }
        }
    };
} // namespace Overseer::Systems

#endif //OVERSEER_WASM_SRC_SYSTEMS_MOVEMENT_MOVEMENTSYSTEM_H_
