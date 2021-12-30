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
#include "InterestMap.h"
#include "core/Pathfinder.h"

namespace Overseer::Systems::AI
{
    class AISystem : SystemBase
    {
      public:
        void Initialise(entt::registry& registry) override
        {
        }

        void Update(entt::registry& registry) override
        {
            auto navMap         = registry.ctx<Core::NavMap>();
            auto friendlyProx   = registry.ctx<Core::FriendlyProxIMAP>();
            auto friendlyThreat = registry.ctx<Core::FriendlyThreatIMAP>();
            auto enemyProx      = registry.ctx<Core::EnemyProxIMAP>();
            auto enemyThreat    = registry.ctx<Core::EnemyThreatIMAP>();

            auto friendlyCreeps =
                registry.view<My, Pos, CreepProxIMAP, CreepThreatIMAP, CreepMovementMap, Threat, Path>();

            for (auto entity : friendlyCreeps)
            {
                if (entity != (entt::entity)16)
                    continue;

                auto [pos, proxIMAP, threatIMAP, movementMap, threat, path] = friendlyCreeps.get(entity);

                printf("1\n");
                // TODO get SL movement map
                auto interestMap = InterestMap(pos.Val,
                                               proxIMAP,
                                               threatIMAP,
                                               movementMap,
                                               movementMap,
                                               friendlyProx,
                                               friendlyThreat,
                                               enemyProx,
                                               enemyThreat,
                                               navMap);
                printf("2\n");
                interestMap.Add(InfluenceType::FriendlyProx, 1.0f);
                printf("3\n");
                interestMap.Add(InfluenceType::MyProx, -1.0f);
                printf("4\n");
                interestMap.Normalise();
                printf("5\n");
                // int2 target = interestMap.GetHighestPos();
                // printf("6: Pos: { %i, %i }    TARGET: { %i, %i }\n", pos.Val.x, pos.Val.y, target.x, target.y);
                int2 target = int2(3, 3);
                Core::MovementCoster::GetPath(
                    PosToIndex(pos.Val, MAP_WIDTH), PosToIndex(target, MAP_WIDTH), movementMap, path);
                printf("7\n");

                auto direction = GetDirection(pos.Val, path.Val[0]);
                printf("8\n");

                CommandHandler::Add(Move((double)entity, (double)direction));
                printf("9\n");
            }
        }

      private:
    };
} // namespace Overseer::Systems::AI
#endif // OVERSEER_WASM_SRC_SYSTEMS_AI_AISYSTEM_H_
