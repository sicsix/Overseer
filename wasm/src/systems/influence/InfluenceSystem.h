//
// Created by Tim on 26/12/2021.
//

#ifndef OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_INFLUENCESYSTEM_H_
#define OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_INFLUENCESYSTEM_H_
#include "systems/SystemBase.h"
#include "components/Components.h"
#include "core/InfluenceMaps.h"
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
                // if (entity != (entt::entity)6)
                //     continue;
                // ranX++;

                auto [pos, proxIMAP, threatIMAP, creepMovementMap, threat] = friendlyCreeps.get(entity);

                // printf("Entity: %i    Pos: { %i, %i }\n", entity, pos.Val.x, pos.Val.y);

                ClampAndSetBounds(pos, proxIMAP, INFLUENCE_PROX_RADIUS, INFLUENCE_PROX_WIDTH);
                ClampAndSetBounds(pos, threatIMAP, INFLUENCE_THREAT_RADIUS, INFLUENCE_THREAT_WIDTH);

                movementCoster.Update(threatIMAP, creepMovementMap);

                ProximityInfluence::Calculate(proxIMAP, creepMovementMap);
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

                ClampAndSetBounds(pos, proxIMAP, INFLUENCE_PROX_RADIUS, INFLUENCE_PROX_WIDTH);
                ClampAndSetBounds(pos, threatIMAP, INFLUENCE_THREAT_RADIUS, INFLUENCE_THREAT_WIDTH);

                movementCoster.Update(threatIMAP, creepMovementMap);

                ProximityInfluence::Calculate(proxIMAP, creepMovementMap);
                ThreatInfluence::Calculate(threatIMAP, lineOfSight, threat, creepMovementMap);

                AddProxInfluence(enemyProx, proxIMAP);
                AddThreatInfluence(enemyThreat, threatIMAP);
            }

            DebugIMAP(friendlyProx, 0.01f, 750);
        }

      private:
        void ClearMap(Core::IMAP imap)
        {
            memset(imap.Influence, 0, sizeof(float) * MAP_SIZE);
        }

        // void SubtractInfluence(Core::IMAP imap, CreepIMAP creepIMAP)
        // {
        //     int mapIndex = creepIMAP.MapStartIndex;
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
            int mapIndex = creepProxIMAP.MapStartIndex;
            int infIndex = creepProxIMAP.InfStartIndex;

            // printf("AddProxInfluence: { ");
            for (int y = creepProxIMAP.InfStart.y; y < creepProxIMAP.InfEnd.y; ++y)
            {
                for (int x = creepProxIMAP.InfStart.x; x < creepProxIMAP.InfEnd.x; ++x)
                {
                    imap.Influence[mapIndex] += creepProxIMAP.Influence[infIndex];
                    // printf("{ Influence: %f, MapIndex: %i, InfIndex: %i }",
                    //        creepProxIMAP.Influence[infIndex],
                    //        mapIndex,
                    //        infIndex);
                    mapIndex++;
                    infIndex++;
                }
                mapIndex += creepProxIMAP.MapYIncrement;
                infIndex += creepProxIMAP.InfYIncrement;
            }
            // printf("} \n");
        }

        void AddThreatInfluence(Core::IMAP imap, CreepThreatIMAP creepThreatIMAP)
        {
            int mapIndex = creepThreatIMAP.MapStartIndex;
            int infIndex = creepThreatIMAP.InfStartIndex;

            // printf("AddThreatInfluence: { ");
            for (int y = creepThreatIMAP.InfStart.y; y < creepThreatIMAP.InfEnd.y; ++y)
            {
                for (int x = creepThreatIMAP.InfStart.x; x < creepThreatIMAP.InfEnd.x; ++x)
                {
                    imap.Influence[mapIndex] += creepThreatIMAP.Influence[infIndex];
                    // printf("{ Influence: %f, MapIndex: %i, InfIndex: %i }",
                    //        creepThreatIMAP.Influence[infIndex],
                    //        mapIndex,
                    //        infIndex);
                    mapIndex++;
                    infIndex++;
                }
                mapIndex += creepThreatIMAP.MapYIncrement;
                infIndex += creepThreatIMAP.InfYIncrement;
            }
            // printf("} \n");
        }

        void ClampAndSetBounds(Pos pos, CreepIMAP& creepIMAP, int influenceRadius, int influenceWidth)
        {
            int2 worldStartUnclamped = pos.Val - influenceRadius;
            int2 worldEndUnclamped   = pos.Val + influenceRadius + 1;

            creepIMAP.WorldStart    = max(worldStartUnclamped, 0);
            creepIMAP.WorldEnd      = min(worldEndUnclamped, MAP_WIDTH);
            creepIMAP.WorldCenter   = pos.Val;
            creepIMAP.MapStartIndex = PosToIndex(creepIMAP.WorldStart, MAP_WIDTH);

            creepIMAP.InfStart      = max(creepIMAP.WorldStart - worldStartUnclamped, 0);
            creepIMAP.InfEnd        = influenceWidth - (worldEndUnclamped - creepIMAP.WorldEnd);
            creepIMAP.InfStartIndex = PosToIndex(creepIMAP.InfStart, influenceWidth);

            // printf(
            //     "  InfStart: { %i, %i }    InfEnd: { %i, %i }    WorldStart: { %i, %i }    WorldEnd: { %i, %i }
            //     WorldStartUnclamped: { %i, %i }    WorldEndUnclamped: { %i, %i }    MapStartIndex: %i InfStartIndex:
            //     %i\n", creepIMAP.InfStart.x, creepIMAP.InfStart.y, creepIMAP.InfEnd.x, creepIMAP.InfEnd.y,
            //     creepIMAP.WorldStart.x,
            //     creepIMAP.WorldStart.y,
            //     creepIMAP.WorldEnd.x,
            //     creepIMAP.WorldEnd.y,
            //     worldStartUnclamped.x,
            //     worldStartUnclamped.y,
            //     worldEndUnclamped.x,
            //     worldEndUnclamped.y,
            //     creepIMAP.MapStartIndex,
            //     creepIMAP.InfStartIndex);

            int width               = creepIMAP.InfEnd.x - creepIMAP.InfStart.x;
            creepIMAP.MapYIncrement = MAP_WIDTH - width;
            creepIMAP.InfYIncrement = influenceWidth - width;
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
                    printf("[WASM] IMAP debug draw limit of %i elements hit\n", maxCount);
                    break;
                }
            }
        }
    };
} // namespace Overseer::Systems::Influence

#endif // OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_INFLUENCESYSTEM_H_
