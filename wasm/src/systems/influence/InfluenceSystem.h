//
// Created by Tim on 26/12/2021.
//

#ifndef OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_INFLUENCESYSTEM_H_
#define OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_INFLUENCESYSTEM_H_
#include "systems/SystemBase.h"
#include "components/Components.h"
#include "core/Math.h"
#include "core/MovementCoster.h"
#include "core/Influence/Influence.h"
namespace Overseer::Systems::Influence
{
    class InfluenceSystem : SystemBase
    {
      public:
        void Initialise(entt::registry& registry) override
        {
            registry.set<FriendlyProxIMAP>();
            registry.set<FriendlyThreatIMAP>();
            registry.set<EnemyProxIMAP>();
            registry.set<EnemyThreatIMAP>();

            InfluencePrecomputes();
        }

        void Update(entt::registry& registry) override
        {
            auto friendlyProx   = registry.ctx<FriendlyProxIMAP>();
            auto friendlyThreat = registry.ctx<FriendlyThreatIMAP>();
            auto enemyProx      = registry.ctx<EnemyProxIMAP>();
            auto enemyThreat    = registry.ctx<EnemyThreatIMAP>();

            auto navMap         = registry.ctx<Core::NavMap>();
            auto movementCoster = registry.ctx<Core::MovementCoster>();
            auto lineOfSight    = registry.ctx<Core::LineOfSight>();

            friendlyProx.Clear();
            friendlyThreat.Clear();
            enemyProx.Clear();
            enemyThreat.Clear();

            auto friendlyCreeps = registry.view<My, Pos, CreepProx, CreepThreat, CreepMovement, Threat>();
            for (auto entity : friendlyCreeps)
            {
                // if (entity != (entt::entity)12)
                //     continue;

                auto [pos, prox, threat, movement, myThreat] = friendlyCreeps.get(entity);

                // printf("Entity: %i    Pos: { %i, %i }\n", entity, pos.Val.x, pos.Val.y);

                movement.ClampAndSetBounds(pos.Val, INTEREST_RADIUS, INTEREST_WIDTH);
                prox.ClampAndSetBounds(pos.Val, INFLUENCE_PROX_RADIUS, INFLUENCE_PROX_WIDTH);
                threat.ClampAndSetBounds(pos.Val, INFLUENCE_THREAT_RADIUS, INFLUENCE_THREAT_WIDTH);

                movementCoster.Update(movement);
                prox.Calculate(navMap);
                threat.Calculate(lineOfSight, myThreat, movement);

                friendlyProx.Add(prox);
                friendlyThreat.Add(threat);
            }

            auto enemyCreeps = registry.view<Pos, CreepProx, CreepThreat, CreepMovement, Threat>(entt::exclude<My>);

            for (auto entity : enemyCreeps)
            {
                // if (entity != (entt::entity) 26)
                //     continue;

                auto [pos, prox, threat, movement, myThreat] = enemyCreeps.get(entity);

                // printf("Entity: %i    Pos: { %i, %i }\n", entity, pos.Val.x, pos.Val.y);

                movement.ClampAndSetBounds(pos.Val, INTEREST_RADIUS, INTEREST_WIDTH);
                prox.ClampAndSetBounds(pos.Val, INFLUENCE_PROX_RADIUS, INFLUENCE_PROX_WIDTH);
                threat.ClampAndSetBounds(pos.Val, INFLUENCE_THREAT_RADIUS, INFLUENCE_THREAT_WIDTH);

                movementCoster.Update(movement);
                prox.Calculate(navMap);
                threat.Calculate(lineOfSight, myThreat, movement);

                enemyProx.Add(prox);
                enemyThreat.Add(threat);
            }

            // DebugIMAP(friendlyThreat, 0.01f, 750);
        }

      private:
    };
} // namespace Overseer::Systems::Influence

#endif // OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_INFLUENCESYSTEM_H_
