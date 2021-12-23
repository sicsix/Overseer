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

namespace Overseer::Systems
{
    class InfluenceSystem : SystemBase
    {
      private:
        ProxSplatMap ProxSplat;

      public:
        void Initialise(entt::registry& registry) override
        {
            registry.set<FriendlyProxIMAP>();
            registry.set<FriendlyThreatIMAP>();
            registry.set<EnemyProxIMAP>();
            registry.set<EnemyThreatIMAP>();

            for (int i = 0; i < INFLUENCE_SIZE; ++i)
            {
            }
        }

        void Update(entt::registry& registry) override
        {
            auto navMap         = registry.ctx<Common::NavMap>();
            auto friendlyProx   = registry.ctx<FriendlyProxIMAP>();
            auto friendlyThreat = registry.ctx<FriendlyThreatIMAP>();
            auto enemyProx      = registry.ctx<FriendlyProxIMAP>();
            auto enemyThreat    = registry.ctx<FriendlyThreatIMAP>();

            ClearMap(friendlyProx);
            ClearMap(friendlyThreat);
            ClearMap(enemyProx);
            ClearMap(enemyThreat);

            // TODO narrow this down eventually to only cover creeps, and all of them
            auto view = registry.view<My, Pos, CreepProxIMAP, CreepThreatIMAP>();

            // bool ranOnce = false;
            for (auto entity : view)
            {
                // if (ranOnce)
                //     break;
                // ranOnce = true;

                auto [pos, prox, threat] = view.get(entity);

                // SubtractInfluence(friendlyProx, prox);
                // SubtractInfluence(friendlyThreat, threat);

                int2 clampedPos   = int2();
                int2 center       = int2();
                int  startIndex   = ClampPosition(pos, clampedPos, center);
                prox.StartIndex   = startIndex;
                threat.StartIndex = startIndex;

                // printf("Entity: %i    Pos: { %i, %i }    ClampedPos: { %i, %i }    Center: { %i, %i }\n",
                //        entity,
                //        pos.Val.x,
                //        pos.Val.y,
                //        clampedPos.x,
                //        clampedPos.y,
                //        center.x,
                //        center.y);

                CalculateProximityInfluence(navMap, prox, center);

                AddInfluence(friendlyProx, prox);
                AddInfluence(friendlyThreat, threat);
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
            int mapIndex = creepIMAP.StartIndex;
            int myIndex  = 0;

            for (int y = 0; y < INFLUENCE_WIDTH; ++y)
            {
                for (int x = 0; x < INFLUENCE_WIDTH; ++x)
                {
                    float influence          = imap.Influence[mapIndex];
                    float creepInfluence     = creepIMAP.Influence[myIndex];
                    float newInfluence       = influence - creepInfluence;
                    newInfluence             = newInfluence < 0.005f ? 0.0f : newInfluence;
                    imap.Influence[mapIndex] = newInfluence;
                    mapIndex++;
                    myIndex++;
                }
                mapIndex += INFLUENCE_MAP_STEP_SIZE;
            }
        }

        void AddInfluence(IMAP imap, CreepIMAP creepIMAP)
        {
            int mapIndex = creepIMAP.StartIndex;
            int myIndex  = 0;

            // printf("{ ");
            for (int y = 0; y < INFLUENCE_WIDTH; ++y)
            {
                for (int x = 0; x < INFLUENCE_WIDTH; ++x)
                {
                    // printf("%f, ", creepIMAP.Influence[myIndex]);
                    imap.Influence[mapIndex] += creepIMAP.Influence[myIndex];
                    mapIndex++;
                    myIndex++;
                }
                mapIndex += INFLUENCE_MAP_STEP_SIZE;
            }
            // printf("} \n");
        }

        int ClampPosition(Pos pos, int2& clampedPos, int2& center)
        {
            clampedPos = clamp(pos.Val, INFLUENCE_CENTER, MAP_WIDTH - INFLUENCE_CENTER);
            center     = int2(INFLUENCE_CENTER, INFLUENCE_CENTER) + (pos.Val - clampedPos);
            return (clampedPos.y - INFLUENCE_CENTER) * MAP_WIDTH + (clampedPos.x - INFLUENCE_CENTER);
        }

        void CalculateProximityInfluence(Common::NavMap navMap, CreepProxIMAP proxIMAP, int2& center)
        {
            // TODO pre calculate the prox splats, need to handle being offset as well

            int mapIndex = proxIMAP.StartIndex;
            int myIndex  = 0;
            // printf("{ ");
            for (int y = 0; y < INFLUENCE_WIDTH; ++y)
            {
                for (int x = 0; x < INFLUENCE_WIDTH; ++x)
                {
                    int   tileCost              = navMap.Map[mapIndex];
                    int   distance              = DistanceChebyshev(center, int2(x, y));
                    float inf                   = CalculateInverse2PolyInfluence(0.5f, distance, INFLUENCE_CENTER);
                    inf                         = tileCost == 255 ? 0 : inf;
                    proxIMAP.Influence[myIndex] = inf;
                    mapIndex++;
                    myIndex++;

                    // printf("%f, ", inf);
                    // printf("{ Dist: %i, Inf: %f }, ", distance, inf);
                }
                mapIndex += INFLUENCE_MAP_STEP_SIZE;
            }
            // printf("} \n");
        }

        void CalculateThreatInfluence()
        {
            // Three types of threat exist -> Heal, Attack, RangedAttack
            // Each should have a different splat and sum them together
            // Do we need to do pathfinding to determine attack threat?
            // Do we need to do visibility checks to determine ranged attack threat?
            // Is heal threat just a constant that ignores vis, but keeps terrain?
            // Maybe set some constants to determine what each part is worth threat wise
            // Attack threat should also depend on ability to move, with no move ability the radius can be 1

            // Is heal a threat? or just an indirect threat? SHould it be included in threat influence?
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
