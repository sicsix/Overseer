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
            auto enemyProx      = registry.ctx<FriendlyProxIMAP>();
            auto enemyThreat    = registry.ctx<FriendlyThreatIMAP>();
            auto pathcoster     = registry.ctx<Common::Pathcoster>();

            ClearMap(friendlyProx);
            ClearMap(friendlyThreat);
            ClearMap(enemyProx);
            ClearMap(enemyThreat);

            // TODO narrow this down eventually to only cover creeps, and all of them
            auto view = registry.view<My, Pos, CreepProxIMAP, CreepThreatIMAP, Threat>();

            // bool ranOnce = false;
            for (auto entity : view)
            {
                // if (ranOnce)
                //     break;
                // ranOnce = true;

                auto [pos, proxIMAP, threatIMAP, threat] = view.get(entity);

                // SubtractInfluence(friendlyProx, proxIMAP);
                // SubtractInfluence(friendlyThreat, threatIMAP);

                ClampAndSetBounds(pos, proxIMAP);
                ClampAndSetBounds(pos, threatIMAP);

                // printf("Entity: %i    Pos: { %i, %i }\n", entity, pos.Val.x, pos.Val.y);

                CalculateProximityInfluence(navMap, proxIMAP);
                CalculateThreatInfluence(navMap, threatIMAP, pathcoster, threat);

                AddInfluence(friendlyProx, proxIMAP);
                AddInfluence(friendlyThreat, threatIMAP);
            }

            // DebugIMAP(friendlyProx, 0.01f, 500);
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

            for (int y = creepIMAP.Start.y; y < creepIMAP.End.y; ++y)
            {
                for (int x = creepIMAP.Start.x; x < creepIMAP.End.x; ++x)
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

            // printf("{ ");
            for (int y = creepIMAP.Start.y; y < creepIMAP.End.y; ++y)
            {
                for (int x = creepIMAP.Start.x; x < creepIMAP.End.x; ++x)
                {
                    imap.Influence[mapIndex] += creepIMAP.Influence[infIndex];
                    // printf("%f, ", creepIMAP.Influence[infIndex]);
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
            int2 clampedPos = clamp(pos.Val, INFLUENCE_CENTER, MAP_WIDTH - INFLUENCE_RADIUS);
            int2 offset     = clampedPos - pos.Val;

            creepIMAP.InfCenter     = INFLUENCE_CENTER + (pos.Val - clampedPos);
            creepIMAP.MapStartIndex = (clampedPos.y - INFLUENCE_RADIUS) * MAP_WIDTH + (clampedPos.x - INFLUENCE_RADIUS);
            creepIMAP.Start         = max(offset, 0);
            creepIMAP.End           = INFLUENCE_WIDTH - min(offset, 0);

            int width = creepIMAP.End.x - creepIMAP.Start.x;

            creepIMAP.InfYIncrement = INFLUENCE_WIDTH - width;
            creepIMAP.MapYIncrement = MAP_WIDTH - width;
            creepIMAP.InfStartIndex = creepIMAP.Start.y * INFLUENCE_WIDTH + creepIMAP.Start.x;
        }

        void CalculateProximityInfluence(Common::NavMap navMap, CreepProxIMAP& proxIMAP)
        {
            // printf(
            //     "Start: { %i, %i }    End: { %i, %i }    Width: %i    WidthDiff: %i    MapIndex: %i    MyIndex:
            //     %i\n", start.x, start.y, end.x, end.y, width, widthDiff, mapIndex, myIndex);

            int mapIndex = proxIMAP.MapStartIndex;
            int infIndex = proxIMAP.InfStartIndex;

            // printf("{ ");
            for (int y = proxIMAP.Start.y; y < proxIMAP.End.y; ++y)
            {
                for (int x = proxIMAP.Start.x; x < proxIMAP.End.x; ++x)
                {
                    int tileCost                 = navMap.Map[mapIndex];
                    proxIMAP.Influence[infIndex] = tileCost == INT_MAXVALUE ? 0 : ProxPrecomputes::Influence[infIndex];
                    // printf("{ MapIndex: %i, MyIndex: %i, Dist: %i, Inf: %f }, ", mapIndex, myIndex, distance, inf);
                    mapIndex++;
                    infIndex++;
                }
                mapIndex += proxIMAP.MapYIncrement;
                infIndex += proxIMAP.InfYIncrement;
            }
            // printf("} \n");
        }

        void CalculateThreatInfluence(Common::NavMap      navMap,
                                      CreepThreatIMAP&    threatIMAP,
                                      Common::Pathcoster& pathcoster,
                                      Threat              threat)
        {
            // Three types of threat exist -> Heal, Attack, RangedAttack
            // Each should have a different splat and sum them together
            // Do we need to do pathfinding to determine attack threat?
            // Do we need to do visibility checks to determine ranged attack threat?
            // Is heal threat just a constant that ignores vis, but keeps terrain?
            // Maybe set some constants to determine what each part is worth threat wise
            // Attack threat should also depend on ability to move, with no move ability the radius can be 1

            // Is heal a threat? or just an indirect threat? SHould it be included in threat influence?

            int costs[INFLUENCE_SIZE + 1];
            // TODO need to make the first move cost 0? so even entities with 0 movement are dangerous, will need to factor in TicksPerMove as well
            pathcoster.GetCosts(costs, threat.TicksPerMove);

            for (int i = 0; i < INFLUENCE_SIZE; ++i)
            {
                int cost = costs[i];
                costs[i] = cost == INT_MAXVALUE ? cost : cost * ;
            }

            int mapIndex = threatIMAP.MapStartIndex;
            int infIndex = threatIMAP.InfStartIndex;

            if (threat.TicksPerMove != -1)
            {
                for (int y = threatIMAP.Start.y; y < threatIMAP.End.y; ++y)
                {
                    for (int x = threatIMAP.Start.x; x < threatIMAP.End.x; ++x)
                    {
                        float inf = ThreatPrecomputes::AttackLookup[min(costs[infIndex], 7)] * threat.AttackDamage * 0.1f;
                        threatIMAP.Influence[infIndex] = inf;
                        mapIndex++;
                        infIndex++;
                    }
                    mapIndex += threatIMAP.MapYIncrement;
                    infIndex += threatIMAP.InfYIncrement;
                }
            }
            else
            {
                // Zero out threat map and draw around creep range 1 the attack threat
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
