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
            auto myCreeps = registry.view<My, Pos, Path>();

            for (auto entity : myCreeps)
            {
                auto [pos, path] = myCreeps.get(entity);

                if (path.Count == 0)
                    continue;

                int2 target = path.Val[0];

                if (pos.Val == target)
                    continue;

                CommandHandler::Add(Move((double)entity, (double)GetDirection(pos.Val, target)));
            }
        }
    };
} // namespace Overseer::Systems

#endif //OVERSEER_WASM_SRC_SYSTEMS_MOVEMENT_MOVEMENTSYSTEM_H_
