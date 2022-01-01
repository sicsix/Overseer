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
            auto pathfinder     = registry.ctx<Core::Pathfinder>();

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
                // int2 target = interestMap.GetHighestPos();

                // Mostly working
                // interestMap.Add(InfluenceType::LeaderMovement, 2.0f);
                // interestMap.Add(InfluenceType::FriendlyProx, -1.0f);
                // interestMap.Add(InfluenceType::MyProx, 1.0f);
                // interestMap.Normalise();
                // interestMap.ApplyInterestTemplate(InterestType::Movement);
                // int2 target = interestMap.GetHighestPos();

                interestMap.Add(InfluenceType::FriendlyProx, 1.0f);
                interestMap.Add(InfluenceType::MyProx, -1.0f);
                interestMap.NormaliseAndInvert();
                interestMap.Multiply(InfluenceType::LeaderMovement, 2.0f);
                interestMap.ApplyInterestTemplate(InterestType::Movement);
                int2 target = interestMap.GetHighestPos();

                if (entity == (entt::entity)10)
                    DebugInterest(interestMap, 0.01f, INTEREST_SIZE);

                // printf("ENTITY: %i    POS: { %i, %i }    TARGET: { %i, %i }\n", entity, pos.Val.x, pos.Val.y, target.x, target.y);
                if (target.x != -1 && target != pos.Val)
                {
                    Core::MovementCoster::GetPath(
                        PosToIndex(pos.Val, MAP_WIDTH), PosToIndex(target, MAP_WIDTH), movement, path);
                }

                // TODO get travel direction and update proximity map with new position
                // TODO potentially update myThreat map too?
            }
        }

      private:
        void DebugInterest(InterestMap interestMap, float minVal, int maxCount)
        {
            int worldIndex = interestMap.WorldStartIndex;
            int localIndex = interestMap.LocalStartIndex;

            int width           = interestMap.LocalEnd.x - interestMap.LocalStart.x;
            int worldYIncrement = MAP_WIDTH - width;
            int localYIncrement = INTEREST_WIDTH - width;

            int count = 0;
            for (int y = interestMap.LocalStart.y; y < interestMap.LocalEnd.y; ++y)
            {
                for (int x = interestMap.LocalStart.x; x < interestMap.LocalEnd.x; ++x)
                {
                    float interest = interestMap.Interest[localIndex];
                    auto  pos      = (double2)IndexToPos(worldIndex, MAP_WIDTH);

                    if (interest > minVal)
                    {
                        count++;
                        CommandHandler::Add(GameVisualRect(pos, { 1.0, 1.0 }, Colour::RED, min(interest, 1.0f)));
                    }

                    if (count >= maxCount)
                    {
                        printf("[WASM] InterestMap debug draw limit of %i elements hit\n", maxCount);
                        break;
                    }

                    worldIndex++;
                    localIndex++;
                }

                worldIndex += worldYIncrement;
                localIndex += localYIncrement;
            }
        }
    };
} // namespace Overseer::Systems::AI
#endif // OVERSEER_WASM_SRC_SYSTEMS_AI_GRUNTSYSTEM_H_
