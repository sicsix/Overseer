//
// Created by Tim on 1/01/2022.
//

#ifndef OVERSEER_WASM_SRC_SYSTEMS_AI_SQUADSYSTEM_H_
#define OVERSEER_WASM_SRC_SYSTEMS_AI_SQUADSYSTEM_H_
#include "systems/SystemBase.h"
#include "components/Components.h"
#include "core/Utility.h"
#include "core/Influence/InterestMap.h"

namespace Overseer::Systems::AI
{

    class SquadSystem : SystemBase
    {
      public:
        void Initialise(entt::registry& registry) override
        {
        }

        void Update(entt::registry& registry) override
        {
            auto friendlyCreeps =
                registry.view<My, Pos, CreepProx, CreepThreat, CreepMovement, Threat, Path>(entt::exclude<SquadLeader>);

            auto squads = registry.view<Squad>();

            if (squads.empty())
                SelectSquadLeader(registry);

            auto pathfinder = registry.ctx<Core::Pathfinder>();
            auto squadLeaders =
                registry.view<My, SquadLeader, Pos, CreepProx, CreepThreat, CreepMovement, Threat, Path>();

            printf("[WASM] GruntSystem: Running squad leaders...\n");
            for (auto entity : squadLeaders)
            {
                auto [squadLeader, pos, prox, threat, movementMap, myThreat, path] = squadLeaders.get(entity);
                auto goal                                                          = int2(90, 90);
                pathfinder.FindPath(pos.Val, goal, path);

                Core::MovementCoster::GetPath(
                    PosToIndex(pos.Val, MAP_WIDTH), PosToIndex(path.Val[0], MAP_WIDTH), movementMap, path);

                // auto squad   = registry.get<Squad>(squadLeader.Squad);
                // int  maxDist = 0;
                // for (int i = 0; i < squad.Size; ++i)
                // {
                //     auto grunt    = squad.Grunts[i];
                //     auto gruntPos = registry.get<Pos>(grunt);
                //     int  dist     = DistanceChebyshev(pos.Val, gruntPos.Val);
                //     if (dist > maxDist)
                //         maxDist = dist;
                // }
                //
                // if (maxDist < 8)
                // {
                //     auto goal = int2(90, 90);
                //     pathfinder.FindPath(pos.Val, goal, path);
                //
                //     if (path.Count > 0 && path.Val[0] != pos.Val)
                //     {
                //         auto direction = GetDirection(pos.Val, path.Val[0]);
                //         CommandHandler::Add(Move((double)entity, (double)direction));
                //     }
                // }
            }
        }

        void SelectSquadLeader(entt::registry& registry)
        {
            auto friendlyCreeps =
                registry.view<My, Pos, CreepProx, CreepThreat, CreepMovement, Threat, Path>(entt::exclude<SquadLeader>);

            auto squads = registry.view<Squad>();

            printf("[WASM] Selecting a squad leader...\n");
            entt::entity bestLeader = entt::entity();

            int highestThreatLevel = std::numeric_limits<int>().min();
            int creepCount         = 0;

            for (auto entity : friendlyCreeps)
            {
                auto threat      = friendlyCreeps.get<Threat>(entity);
                int  threatLevel = Core::Utility::CalculateThreatLevel(threat);
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
            printf("[WASM] Selected %i...\n", bestLeader);
        }
    };
} // namespace Overseer::Systems::AI
#endif //OVERSEER_WASM_SRC_SYSTEMS_AI_SQUADSYSTEM_H_
