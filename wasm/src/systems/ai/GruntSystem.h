//
// Created by Tim on 30/12/2021.
//

#ifndef OVERSEER_WASM_SRC_SYSTEMS_AI_GRUNTSYSTEM_H_
#define OVERSEER_WASM_SRC_SYSTEMS_AI_GRUNTSYSTEM_H_
#include "systems/SystemBase.h"
#include "components/Components.h"
#include "commands/CommandHandler.h"
#include "core/Math.h"
#include "core/MovementCoster.h"
#include "core/Influence/Influence.h"
#include "core/Influence/InterestMap.h"
#include "core/Pathfinder.h"

namespace Overseer::Systems::AI
{
    class GruntSystem : SystemBase
    {
      public:
        void Initialise(entt::registry& registry) override
        {
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

            auto friendlyCreeps =
                registry.view<My, Pos, CreepProx, CreepThreat, CreepMovement, Threat, Path>(entt::exclude<SquadLeader>);

            auto grunts = registry.view<My, SquadRef, Pos, CreepProx, CreepThreat, CreepMovement, Threat, Path>(
                entt::exclude<SquadLeader>);

            for (auto entity : grunts)
            {
                // if (entity != (entt::entity)10)
                //     continue;

                auto [squadRef, pos, prox, threat, movement, myThreat, path] = grunts.get(entity);

                auto squadLeader            = squadRef.SquadLeader;
                auto squadLeaderMovementMap = registry.get<CreepMovement>(squadLeader);

                // TODO get SL movement map
                auto interestMap = InterestMap(pos.Val,
                                               prox,
                                               threat,
                                               movement,
                                               squadLeaderMovementMap,
                                               friendlyProx,
                                               friendlyThreat,
                                               enemyProx,
                                               enemyThreat,
                                               navMap);

                // interestMap.Add(InfluenceType::FriendlyProx, 1.0f);
                // interestMap.Add(InfluenceType::MyProx, -1.0f);
                // interestMap.NormaliseAndInvert();
                // interestMap.ApplyInterestTemplate(InterestType::Movement);
                // int2 highestPos = interestMap.GetHighestPos();

                // Mostly working
                // interestMap.Add(InfluenceType::LeaderMovement, 2.0f);
                // interestMap.Add(InfluenceType::FriendlyProx, -1.0f);
                // interestMap.Add(InfluenceType::MyProx, 1.0f);
                // interestMap.Normalise();
                // interestMap.ApplyInterestTemplate(InterestType::Movement);
                // int2 highestPos = interestMap.GetHighestPos();

                interestMap.Add(InfluenceType::FriendlyProx, 1.0f);
                interestMap.Add(InfluenceType::MyProx, -1.0f);
                interestMap.NormaliseAndInvert(0.75f, 1.0f);
                interestMap.Multiply(InfluenceType::LeaderMovement, 2.0f);
                interestMap.Add(InfluenceType::EnemyThreat, 1.0f);
                interestMap.ApplyInterestTemplate(InterestType::Movement);
                int2 highestPos = interestMap.GetHighestPos();

                if (entity == (entt::entity)10)
                    interestMap.DebugDraw(0.01f, INTEREST_SIZE);

                // printf("ENTITY: %i    POS: { %i, %i }    TARGET: { %i, %i }\n", entity, pos.Val.x, pos.Val.y, highestPos.x, highestPos.y);
                if (highestPos.x == -1 || highestPos == pos.Val)
                    continue;

                Core::MovementCoster::GetPath(
                    PosToIndex(pos.Val, MAP_WIDTH), PosToIndex(highestPos, MAP_WIDTH), movement, path);

                int2 target = path.Val[0];

                friendlyProx.Subtract(prox);
                friendlyThreat.Subtract(threat);

                movement.ClampAndSetBounds(target, INTEREST_RADIUS, INTEREST_WIDTH);
                prox.ClampAndSetBounds(target, INFLUENCE_PROX_RADIUS, INFLUENCE_PROX_WIDTH);
                threat.ClampAndSetBounds(target, INFLUENCE_THREAT_RADIUS, INFLUENCE_THREAT_WIDTH);

                movementCoster.Update(movement);
                prox.Calculate(navMap);
                threat.Calculate(lineOfSight, myThreat, movement);

                friendlyProx.Add(prox);
                friendlyThreat.Add(threat);
            }
        }
    };
} // namespace Overseer::Systems::AI
#endif // OVERSEER_WASM_SRC_SYSTEMS_AI_GRUNTSYSTEM_H_
