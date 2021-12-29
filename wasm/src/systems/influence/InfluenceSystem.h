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

namespace Overseer::Systems
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

            ProxPrecomputes();
            ThreatPrecomputes();
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

                CalculateProximityInfluence(navMap, proxIMAP);
                CalculateThreatInfluence(navMap, threatIMAP, pathcoster, lineOfSight, threat);

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

                CalculateProximityInfluence(navMap, proxIMAP);
                CalculateThreatInfluence(navMap, threatIMAP, pathcoster, lineOfSight, threat);

                AddInfluence(enemyProx, proxIMAP);
                AddInfluence(enemyThreat, threatIMAP);
            }

            // DebugIMAP(enemyProx, 0.01f, 500);
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

        void CalculateProximityInfluence(Common::NavMap navMap, CreepProxIMAP& proxIMAP)
        {
            // printf(
            //     "WorldStart: { %i, %i }    WorldEnd: { %i, %i }    WorldCenter: { %i, %i }    MapStartIndex: %i
            //     MapYIncrement: %i    InfStart: { %i, %i }    InfEnd: { %i, %i }    InfStartIndex: %i InfYIncrement:
            //     %i\n", proxIMAP.WorldStart.x, proxIMAP.WorldStart.y, proxIMAP.WorldEnd.x, proxIMAP.WorldEnd.y,
            //     proxIMAP.WorldCenter.x,
            //     proxIMAP.WorldCenter.y,
            //     proxIMAP.MapStartIndex,
            //     proxIMAP.MapYIncrement,
            //     proxIMAP.InfStart.x,
            //     proxIMAP.InfStart.y,
            //     proxIMAP.InfEnd.x,
            //     proxIMAP.InfEnd.y,
            //     proxIMAP.InfStartIndex,
            //     proxIMAP.InfYIncrement);

            int mapIndex = proxIMAP.MapStartIndex;
            int infIndex = proxIMAP.InfStartIndex;

            // printf("CalculateProximityInfluence: { ");
            for (int y = proxIMAP.InfStart.y; y < proxIMAP.InfEnd.y; ++y)
            {
                for (int x = proxIMAP.InfStart.x; x < proxIMAP.InfEnd.x; ++x)
                {
                    int   tileCost               = navMap.Map[mapIndex];
                    float inf                    = ProxPrecomputes::Influence[infIndex];
                    proxIMAP.Influence[infIndex] = tileCost == INT_MAXVALUE ? 0 : inf;
                    // printf("{ MapIndex: %i, MyIndex: %i, Inf: %f }, ", mapIndex, infIndex, inf);
                    mapIndex++;
                    infIndex++;
                }
                mapIndex += proxIMAP.MapYIncrement;
                infIndex += proxIMAP.InfYIncrement;
            }
            // printf("} \n");
        }

        void CalculateThreatInfluence(Common::NavMap       navMap,
                                      CreepThreatIMAP&     threatIMAP,
                                      Common::Pathcoster&  pathcoster,
                                      Common::LineOfSight& lineOfSight,
                                      Threat               threat)
        {
            // Three types of threat exist -> Heal, Attack, RangedAttack
            // Each should have a different splat and sum them together
            // Do we need to do pathfinding to determine attack threat?
            // Do we need to do visibility checks to determine ranged attack threat?
            // Is heal threat just a constant that ignores vis, but keeps terrain?
            // Maybe set some constants to determine what each part is worth threat wise
            // Attack threat should also depend on ability to move, with no move ability the radius can be 1

            // Is heal a threat? or just an indirect threat? SHould it be included in threat influence?

            // Calcalute ATTACK threat
            int array[INFLUENCE_SIZE + 1];

            if (threat.AttackDamage > 0)
            {
                pathcoster.GetAttackCosts(threatIMAP, threat.TicksPerMove, array);

                int mapIndex = threatIMAP.MapStartIndex;
                int infIndex = threatIMAP.InfStartIndex;

                // printf("CalculateAttackThreatInfluence: { ");
                for (int y = threatIMAP.InfStart.y; y < threatIMAP.InfEnd.y; ++y)
                {
                    for (int x = threatIMAP.InfStart.x; x < threatIMAP.InfEnd.x; ++x)
                    {
                        int   cost                     = array[infIndex];
                        float distBase                 = ThreatPrecomputes::AttackLookup[min(cost, 7)];
                        float inf                      = distBase * threat.AttackDamage * INFLUENCE_ATTACK_THREAT_MULT;
                        threatIMAP.Influence[infIndex] = inf;
                        // printf("{ MapIndex: %i, InfIndex: %i, Cost: %i, DistBase: %f, Inf: %f }, ",
                        //        mapIndex,
                        //        infIndex,
                        //        cost,
                        //        distBase,
                        //        inf);
                        mapIndex++;
                        infIndex++;
                    }
                    mapIndex += threatIMAP.MapYIncrement;
                    infIndex += threatIMAP.InfYIncrement;
                }
                // printf("} \n");
            }
            else
                memset(threatIMAP.Influence, 0, sizeof(float) * INFLUENCE_SIZE);

            if (threat.RangedAttackDamage > 0 || threat.RangedHealRate > 0)
                lineOfSight.CalculateLOS(threatIMAP, array);

            // Calcalute RANGED threat
            if (threat.RangedAttackDamage > 0)
            {
                int mapIndex = threatIMAP.MapStartIndex;
                int infIndex = threatIMAP.InfStartIndex;

                // TODO handle non moving creeps

                // printf("CalculateRangedAttackThreatInfluence: { ");
                for (int y = threatIMAP.InfStart.y; y < threatIMAP.InfEnd.y; ++y)
                {
                    for (int x = threatIMAP.InfStart.x; x < threatIMAP.InfEnd.x; ++x)
                    {
                        int   inLos      = array[infIndex];
                        float threatBase = inLos ? ThreatPrecomputes::RangedAttack4Falloff[infIndex] : 0;
                        float inf        = threatBase * threat.RangedAttackDamage * INFLUENCE_RANGED_ATTACK_THREAT_MULT;
                        threatIMAP.Influence[infIndex] += inf;
                        // if (inf > 0)
                        // {
                        //     // printf("{ MapIndex: %i, InfIndex: %i, InLos: %i, ThreatBase: %f, Inf: %f }, ",
                        //            mapIndex,
                        //            infIndex,
                        //            inLos,
                        //            threatBase,
                        //            inf);
                        // }
                        mapIndex++;
                        infIndex++;
                    }
                    mapIndex += threatIMAP.MapYIncrement;
                    infIndex += threatIMAP.InfYIncrement;
                }
                // printf("} \n");
            }

            if (threat.RangedHealRate > 0)
            {
                // TODO this
            }
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
