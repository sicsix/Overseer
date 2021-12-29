//
// Created by Tim on 26/12/2021.
//

#ifndef OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_INFLUENCESYSTEM_H_
#define OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_INFLUENCESYSTEM_H_
#include "systems/SystemBase.h"
#include "components/Components.h"
#include "InfluenceMaps.h"
#include "commands/CommandHandler.h"
#include "common/Math.h"
#include "common/Pathcoster.h"
#include "ThreatInfluence.h"
#include "ProximityInfluence.h"

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
            auto navMap         = registry.ctx<Common::NavMap>();
            auto friendlyProx   = registry.ctx<FriendlyProxIMAP>();
            auto friendlyThreat = registry.ctx<FriendlyThreatIMAP>();
            auto enemyProx      = registry.ctx<EnemyProxIMAP>();
            auto enemyThreat    = registry.ctx<EnemyThreatIMAP>();
            auto pathcoster     = registry.ctx<Common::Pathcoster>();
            auto lineOfSight    = registry.ctx<Common::LineOfSight>();

            ClearMap(friendlyProx);
            ClearMap(friendlyThreat);
            ClearMap(enemyProx);
            ClearMap(enemyThreat);

            int costs[INFLUENCE_SIZE + 1];

            auto friendlyCreeps = registry.view<My, Pos, CreepProxIMAP, CreepThreatIMAP, Threat>();
            // int  ranX           = 0;
            for (auto entity : friendlyCreeps)
            {
                // if (entity != (entt::entity)6)
                //     continue;
                // ranX++;

                auto [pos, proxIMAP, threatIMAP, threat] = friendlyCreeps.get(entity);

                // printf("Entity: %i    Pos: { %i, %i }\n", entity, pos.Val.x, pos.Val.y);

                // SubtractInfluence(friendlyProx, proxIMAP);
                // SubtractInfluence(friendlyThreat, threatIMAP);

                ClampAndSetBounds(pos, proxIMAP);
                ClampAndSetBounds(pos, threatIMAP);


                pathcoster.GetPathingCosts(threatIMAP, threat.TicksPerMove, costs);

                CalculateProximityInfluence(proxIMAP, costs);
                CalculateThreatInfluence(threatIMAP, lineOfSight, threat, costs);

                AddInfluence(friendlyProx, proxIMAP);
                AddInfluence(friendlyThreat, threatIMAP);
            }

            auto enemyCreeps = registry.view<Pos, CreepProxIMAP, CreepThreatIMAP, Threat>(entt::exclude<My>);

            for (auto entity : enemyCreeps)
            {
                auto [pos, proxIMAP, threatIMAP, threat] = enemyCreeps.get(entity);

                // printf("Entity: %i    Pos: { %i, %i }\n", entity, pos.Val.x, pos.Val.y);

                ClampAndSetBounds(pos, proxIMAP);
                ClampAndSetBounds(pos, threatIMAP);

                pathcoster.GetPathingCosts(threatIMAP, threat.TicksPerMove, costs);

                CalculateProximityInfluence(proxIMAP, costs);
                CalculateThreatInfluence(threatIMAP, lineOfSight, threat, costs);

                AddInfluence(enemyProx, proxIMAP);
                AddInfluence(enemyThreat, threatIMAP);
            }

            DebugIMAP(friendlyThreat, 0.01f, 500);
        }

      private:
        void ClearMap(IMAP imap)
        {
            memset(imap.Influence, 0, sizeof(float) * MAP_SIZE);
        }

        void SubtractInfluence(IMAP imap, CreepIMAP creepIMAP)
        {
            int mapIndex = creepIMAP.MapStartIndex;
            int infIndex = creepIMAP.InfStartIndex;

            for (int y = creepIMAP.InfStart.y; y < creepIMAP.InfEnd.y; ++y)
            {
                for (int x = creepIMAP.InfStart.x; x < creepIMAP.InfEnd.x; ++x)
                {
                    float influence          = imap.Influence[mapIndex];
                    float creepInfluence     = creepIMAP.Influence[infIndex];
                    float newInfluence       = influence - creepInfluence;
                    newInfluence             = newInfluence < 0.005f ? 0.0f : newInfluence;
                    imap.Influence[mapIndex] = newInfluence;
                    mapIndex++;
                    infIndex++;
                }
                mapIndex += creepIMAP.MapYIncrement;
                infIndex += creepIMAP.InfYIncrement;
            }
        }

        void AddInfluence(IMAP imap, CreepIMAP creepIMAP)
        {
            int mapIndex = creepIMAP.MapStartIndex;
            int infIndex = creepIMAP.InfStartIndex;

            // printf("AddInfluence: { ");
            for (int y = creepIMAP.InfStart.y; y < creepIMAP.InfEnd.y; ++y)
            {
                for (int x = creepIMAP.InfStart.x; x < creepIMAP.InfEnd.x; ++x)
                {
                    imap.Influence[mapIndex] += creepIMAP.Influence[infIndex];
                    // printf("{ Influence: %f, MapIndex: %i, InfIndex: %i }",
                    //        creepIMAP.Influence[infIndex],
                    //        mapIndex,
                    //        infIndex);
                    mapIndex++;
                    infIndex++;
                }
                mapIndex += creepIMAP.MapYIncrement;
                infIndex += creepIMAP.InfYIncrement;
            }
            // printf("} \n");
        }

        void ClampAndSetBounds(Pos pos, CreepIMAP& creepIMAP)
        {
            int2 worldStartUnclamped = pos.Val - INFLUENCE_RADIUS;
            int2 worldEndUnclamped   = pos.Val + INFLUENCE_RADIUS + 1;

            creepIMAP.WorldStart          = max(worldStartUnclamped, 0);
            creepIMAP.WorldEnd            = min(worldEndUnclamped, MAP_WIDTH);
            creepIMAP.WorldCenter         = pos.Val;
            creepIMAP.MapStartIndex       = PosToIndex(creepIMAP.WorldStart, MAP_WIDTH);

            creepIMAP.InfStart      = max(creepIMAP.WorldStart - worldStartUnclamped, 0);
            creepIMAP.InfEnd        = INFLUENCE_WIDTH - (worldEndUnclamped - creepIMAP.WorldEnd);
            creepIMAP.InfStartIndex = PosToIndex(creepIMAP.InfStart, INFLUENCE_WIDTH);

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
            creepIMAP.InfYIncrement = INFLUENCE_WIDTH - width;
        }





        void DebugIMAP(IMAP imap, float minVal, int maxCount)
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
} // namespace Overseer::Systems

#endif // OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_INFLUENCESYSTEM_H_
