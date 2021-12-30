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

        void Update(entt::registry& registry) override
        {
            auto navMap         = registry.ctx<Core::NavMap>();
            auto friendlyProx   = registry.ctx<Core::FriendlyProxIMAP>();
            auto friendlyThreat = registry.ctx<Core::FriendlyThreatIMAP>();
            auto enemyProx      = registry.ctx<Core::EnemyProxIMAP>();
            auto enemyThreat    = registry.ctx<Core::EnemyThreatIMAP>();

            auto friendlyCreeps =
                registry.view<My, Pos, CreepProxIMAP, CreepThreatIMAP, CreepMovementMap, Threat, Path>();

            for (auto entity : friendlyCreeps)
            {
                // if (entity != (entt::entity)10)
                //     continue;

                auto [pos, proxIMAP, threatIMAP, movementMap, threat, path] = friendlyCreeps.get(entity);

                // TODO get SL movement map
                auto interestMap = InterestMap(pos.Val,
                                               proxIMAP,
                                               threatIMAP,
                                               movementMap,
                                               movementMap,
                                               friendlyProx,
                                               friendlyThreat,
                                               enemyProx,
                                               enemyThreat,
                                               navMap);

                interestMap.Add(InfluenceType::FriendlyProx, 1.0f);
                interestMap.Add(InfluenceType::MyProx, -1.0f);
                interestMap.Normalise();
                interestMap.ApplyInterestTemplate(InterestType::Proximity);
                int2 target = interestMap.GetHighestPos();
                // TODO highest pos seems to be returning values outside of range? Check northmost Creep 10
                // target      = int2(8, 10);

                // DebugInterest(interestMap, 0.01f, INTEREST_SIZE);

                // TODO HANDLE NOT RETURNING A REAL TARGET? OTHERWISE WE CRASH! IE WITH NO NEARBY CREEPS IT JUS DIES

                // printf("6: Pos: { %i, %i }    TARGET: { %i, %i }\n", pos.Val.x, pos.Val.y, target.x, target.y);
                // target = int2(3, 3);
                Core::MovementCoster::GetPath(
                    PosToIndex(pos.Val, MAP_WIDTH), PosToIndex(target, MAP_WIDTH), movementMap, path);

                // TODO Should return no path if target == same, check path not zero length before doing below
                auto direction = GetDirection(pos.Val, path.Val[0]);
                CommandHandler::Add(Move((double)entity, (double)direction));
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
