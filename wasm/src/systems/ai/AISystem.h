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

        int CalculateThreatLevel(Threat& threat)
        {
            // TODO deadshit way of doing this and doesnt belong here
            return threat.AttackDamage + threat.RangedAttackDamage + threat.HealRate;
        }

        void Update(entt::registry& registry) override
        {
            auto navMap         = registry.ctx<Core::NavMap>();
            auto friendlyProx   = registry.ctx<Core::FriendlyProxIMAP>();
            auto friendlyThreat = registry.ctx<Core::FriendlyThreatIMAP>();
            auto enemyProx      = registry.ctx<Core::EnemyProxIMAP>();
            auto enemyThreat    = registry.ctx<Core::EnemyThreatIMAP>();
            auto pathfinder     = registry.ctx<Core::Pathfinder>();

            auto friendlyCreeps =
                registry.view<My, Pos, CreepProxIMAP, CreepThreatIMAP, CreepMovementMap, Threat, Path>(
                    entt::exclude<SquadLeader>);

            auto squads = registry.view<Squad>();

            if (squads.empty())
            {
                printf("Selecting a squad leader...\n");
                entt::entity bestLeader = entt::entity();

                int highestThreatLevel = std::numeric_limits<int>().min();
                int creepCount         = 0;

                for (auto entity : friendlyCreeps)
                {
                    auto threat      = friendlyCreeps.get<Threat>(entity);
                    int  threatLevel = CalculateThreatLevel(threat);
                    creepCount++;

                    if (threatLevel < highestThreatLevel)
                        continue;

                    highestThreatLevel = threatLevel;
                    bestLeader         = entity;
                }

                auto squadEntity = registry.create((entt::entity)100000);

                auto squad = Squad();
                for (auto entity : friendlyCreeps)
                {
                    if (entity == bestLeader)
                        continue;

                    squad.Grunts[squad.Size++] = entity;
                    registry.emplace<SquadRef>(entity, SquadRef(squadEntity, bestLeader));
                }


                registry.emplace<Squad>(squadEntity, squad);
                registry.emplace<SquadLeader>(bestLeader, SquadLeader(squadEntity));
                printf("Selected %i...\n", bestLeader);
            }

            auto squadLeaders =
                registry.view<My, SquadLeader, Pos, CreepProxIMAP, CreepThreatIMAP, CreepMovementMap, Threat, Path>();

            for (auto entity : squadLeaders)
            {
                printf("Running squad leaders...\n");
                auto [squadLeader, pos, proxIMAP, threatIMAP, movementMap, threat, path] = squadLeaders.get(entity);

                auto goal = int2(90, 90);
                pathfinder.FindPath(pos.Val, goal, path);

                if (path.Val[0] != pos.Val)
                {
                    Core::MovementCoster::GetPath(
                        PosToIndex(pos.Val, MAP_WIDTH), PosToIndex(path.Val[0], MAP_WIDTH), movementMap, path);
                    auto direction = GetDirection(pos.Val, path.Val[0]);
                    CommandHandler::Add(Move((double)entity, (double)direction));
                }
            }

            auto grunts = registry.view<My, SquadRef, Pos, CreepProxIMAP, CreepThreatIMAP, CreepMovementMap, Threat, Path>(
                entt::exclude<SquadLeader>);

            for (auto entity : grunts)
            {
                if (entity != (entt::entity)10)
                    continue;

                auto [squadRef, pos, proxIMAP, threatIMAP, movementMap, threat, path] = grunts.get(entity);

                auto squadLeader = squadRef.SquadLeader;
                auto squadLeaderMovementMap = registry.get<CreepMovementMap>(squadLeader);

                // TODO get SL movement map
                auto interestMap = InterestMap(pos.Val,
                                               proxIMAP,
                                               threatIMAP,
                                               movementMap,
                                               squadLeaderMovementMap,
                                               friendlyProx,
                                               friendlyThreat,
                                               enemyProx,
                                               enemyThreat,
                                               navMap);

                printf("Interest 1");
                // interestMap.Add(InfluenceType::FriendlyProx, 1.0f);
                // interestMap.Add(InfluenceType::MyProx, -1.0f);
                // interestMap.NormaliseAndInvert();
                // interestMap.ApplyInterestTemplate(InterestType::MovementMap);
                // int2 target = interestMap.GetHighestPos();

                interestMap.Add(InfluenceType::SquadLeaderMovementMap, 2.0f);
                printf("Interest 2");
                // interestMap.ApplyInterestTemplate(InterestType::MovementMap);
                int2 target = interestMap.GetHighestPos();

                if (entity == (entt::entity)10)
                    DebugInterest(interestMap, 0.01f, INTEREST_SIZE);

                // TODO HANDLE NOT RETURNING A REAL TARGET? OTHERWISE WE CRASH! IE WITH NO NEARBY CREEPS IT JUS DIES

                // printf("6: Pos: { %i, %i }    TARGET: { %i, %i }\n", pos.Val.x, pos.Val.y, target.x, target.y);
                // if (target != pos.Val)
                // {
                //     Core::MovementCoster::GetPath(
                //         PosToIndex(pos.Val, MAP_WIDTH), PosToIndex(target, MAP_WIDTH), movementMap, path);
                //     auto direction = GetDirection(pos.Val, path.Val[0]);
                //     CommandHandler::Add(Move((double)entity, (double)direction));
                // }
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
#endif // OVERSEER_WASM_SRC_SYSTEMS_AI_AISYSTEM_H_
