//
// Created by Tim on 26/12/2021.
//

#ifndef OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_INFLUENCESYSTEM_H_
#define OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_INFLUENCESYSTEM_H_
#include "systems/SystemBase.h"
#include "components/Components.h"
#include "core/Influence.h"
#include "commands/CommandHandler.h"
#include "core/Math.h"
#include "core/MovementCoster.h"
#include "InfluencePrecomputes.h"
#include "ThreatInfluence.h"
#include "ProximityInfluence.h"

namespace Overseer::Systems::Influence
{
    class InfluenceSystem : SystemBase
    {
      public:
        void Initialise(entt::registry& registry) override
        {
            registry.set<Core::FriendlyProxIMAP>();
            registry.set<Core::FriendlyThreatIMAP>();
            registry.set<Core::EnemyProxIMAP>();
            registry.set<Core::EnemyThreatIMAP>();

            InfluencePrecomputes();
        }

        void Update(entt::registry& registry) override
        {
            auto navMap         = registry.ctx<Core::NavMap>();
            auto friendlyProx   = registry.ctx<Core::FriendlyProxIMAP>();
            auto friendlyThreat = registry.ctx<Core::FriendlyThreatIMAP>();
            auto enemyProx      = registry.ctx<Core::EnemyProxIMAP>();
            auto enemyThreat    = registry.ctx<Core::EnemyThreatIMAP>();
            auto movementCoster = registry.ctx<Core::MovementCoster>();
            auto lineOfSight    = registry.ctx<Core::LineOfSight>();

            ClearMap(friendlyProx);
            ClearMap(friendlyThreat);
            ClearMap(enemyProx);
            ClearMap(enemyThreat);

            int costs[INFLUENCE_THREAT_SIZE + 1];

            auto friendlyCreeps = registry.view<My, Pos, CreepProxIMAP, CreepThreatIMAP, CreepMovementMap, Threat>();
            // int  ranX           = 0;
            for (auto entity : friendlyCreeps)
            {
                if (entity != (entt::entity)16)
                    continue;
                // ranX++;

                auto [pos, proxIMAP, threatIMAP, creepMovementMap, threat] = friendlyCreeps.get(entity);

                // printf("Entity: %i    Pos: { %i, %i }\n", entity, pos.Val.x, pos.Val.y);

                proxIMAP.ClampAndSetBounds(pos.Val, INFLUENCE_PROX_RADIUS, INFLUENCE_PROX_WIDTH);
                threatIMAP.ClampAndSetBounds(pos.Val, INFLUENCE_THREAT_RADIUS, INFLUENCE_THREAT_WIDTH);
                creepMovementMap.ClampAndSetBounds(pos.Val, INTEREST_RADIUS, INTEREST_WIDTH);
                movementCoster.Update(creepMovementMap);

                ProximityInfluence::Calculate(proxIMAP, navMap);
                ThreatInfluence::Calculate(threatIMAP, lineOfSight, threat, creepMovementMap);

                AddProxInfluence(friendlyProx, proxIMAP);
                AddThreatInfluence(friendlyThreat, threatIMAP);
            }

            auto enemyCreeps =
                registry.view<Pos, CreepProxIMAP, CreepThreatIMAP, CreepMovementMap, Threat>(entt::exclude<My>);

            for (auto entity : enemyCreeps)
            {
                auto [pos, proxIMAP, threatIMAP, creepMovementMap, threat] = enemyCreeps.get(entity);

                // printf("Entity: %i    Pos: { %i, %i }\n", entity, pos.Val.x, pos.Val.y);

                proxIMAP.ClampAndSetBounds(pos.Val, INFLUENCE_PROX_RADIUS, INFLUENCE_PROX_WIDTH);
                threatIMAP.ClampAndSetBounds(pos.Val, INFLUENCE_THREAT_RADIUS, INFLUENCE_THREAT_WIDTH);
                creepMovementMap.ClampAndSetBounds(pos.Val, INTEREST_RADIUS, INTEREST_WIDTH);
                movementCoster.Update(creepMovementMap);

                ProximityInfluence::Calculate(proxIMAP, navMap);
                ThreatInfluence::Calculate(threatIMAP, lineOfSight, threat, creepMovementMap);

                AddProxInfluence(enemyProx, proxIMAP);
                AddThreatInfluence(enemyThreat, threatIMAP);
            }

            DebugIMAP(friendlyThreat, 0.01f, 750);
        }

      private:
        void ClearMap(Core::IMAP imap)
        {
            memset(imap.Influence, 0, sizeof(float) * MAP_SIZE);
        }

        // void SubtractInfluence(Core::LocalMap imap, LocalMap creepIMAP)
        // {
        //     int mapIndex = creepIMAP.WorldStartIndex;
        //     int infIndex = creepIMAP.InfStartIndex;
        //
        //     for (int y = creepIMAP.InfStart.y; y < creepIMAP.InfEnd.y; ++y)
        //     {
        //         for (int x = creepIMAP.InfStart.x; x < creepIMAP.InfEnd.x; ++x)
        //         {
        //             float influence          = imap.Influence[mapIndex];
        //             float creepInfluence     = creepIMAP.Influence[infIndex];
        //             float newInfluence       = influence - creepInfluence;
        //             newInfluence             = newInfluence < 0.005f ? 0.0f : newInfluence;
        //             imap.Influence[mapIndex] = newInfluence;
        //             mapIndex++;
        //             infIndex++;
        //         }
        //         mapIndex += creepIMAP.MapYIncrement;
        //         infIndex += creepIMAP.InfYIncrement;
        //     }
        // }

        void AddProxInfluence(Core::IMAP imap, CreepProxIMAP creepProxIMAP)
        {
            int worldIndex = creepProxIMAP.WorldStartIndex;
            int localIndex = creepProxIMAP.LocalStartIndex;

            int width           = creepProxIMAP.LocalEnd.x - creepProxIMAP.LocalStart.x;
            int worldYIncrement = MAP_WIDTH - width;
            int localYIncrement = INFLUENCE_PROX_WIDTH - width;

            // printf("AddProxInfluence: { ");
            for (int y = creepProxIMAP.LocalStart.y; y < creepProxIMAP.LocalEnd.y; ++y)
            {
                for (int x = creepProxIMAP.LocalStart.x; x < creepProxIMAP.LocalEnd.x; ++x)
                {
                    imap.Influence[worldIndex] += creepProxIMAP.Influence[localIndex];
                    // printf("{ Influence: %f, MapIndex: %i, InfIndex: %i }",
                    //        creepProxIMAP.Influence[infIndex],
                    //        mapIndex,
                    //        infIndex);
                    worldIndex++;
                    localIndex++;
                }
                worldIndex += worldYIncrement;
                localIndex += localYIncrement;
            }
            // printf("} \n");
        }

        void AddThreatInfluence(Core::IMAP imap, CreepThreatIMAP creepThreatIMAP)
        {
            int worldIndex = creepThreatIMAP.WorldStartIndex;
            int localIndex = creepThreatIMAP.LocalStartIndex;

            int width           = creepThreatIMAP.LocalEnd.x - creepThreatIMAP.LocalStart.x;
            int worldYIncrement = MAP_WIDTH - width;
            int localYIncrement = INFLUENCE_THREAT_WIDTH - width;

            // printf("AddThreatInfluence: { ");
            for (int y = creepThreatIMAP.LocalStart.y; y < creepThreatIMAP.LocalEnd.y; ++y)
            {
                for (int x = creepThreatIMAP.LocalStart.x; x < creepThreatIMAP.LocalEnd.x; ++x)
                {
                    imap.Influence[worldIndex] += creepThreatIMAP.Influence[localIndex];
                    // printf("{ Influence: %f, MapIndex: %i, InfIndex: %i }",
                    //        creepThreatIMAP.Influence[infIndex],
                    //        mapIndex,
                    //        infIndex);
                    worldIndex++;
                    localIndex++;
                }
                worldIndex += worldYIncrement;
                localIndex += localYIncrement;
            }
            // printf("} \n");
        }

        void DebugIMAP(Core::IMAP imap, float minVal, int maxCount)
        {
            int count = 0;
            for (int i = 0; i < MAP_SIZE; ++i)
            {
                float influence = imap.Influence[i];

                auto pos = double2(i % MAP_WIDTH, i / MAP_WIDTH);

                if (influence > minVal)
                {
                    count++;
                    CommandHandler::Add(GameVisualRect(pos, { 1.0, 1.0 }, Colour::RED, min(influence, 1.0f)));
                }

                if (count >= maxCount)
                {
                    printf("[WASM] LocalMap debug draw limit of %i elements hit\n", maxCount);
                    break;
                }
            }
        }
    };
} // namespace Overseer::Systems::Influence

#endif // OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_INFLUENCESYSTEM_H_
