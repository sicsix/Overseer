//
// Created by Tim on 30/12/2021.
//

#ifndef OVERSEER_WASM_SRC_SYSTEMS_AI_INTERESTMAP_H_
#define OVERSEER_WASM_SRC_SYSTEMS_AI_INTERESTMAP_H_
#include "Includes.h"
#include "core/Math.h"
#include "core/InfluenceMaps.h"
#include "components/Components.h"

namespace Overseer::Systems::AI
{
    enum struct InfluenceType
    {
        MyProx,
        MyThreat,
        FriendlyProx,
        FriendlyThreat,
        EnemyProx,
        EnemyThreat
    };

    enum struct InterestType
    {
        Distance, // Distance with no pathfinding
        Proximity // With pathfinding - perhaps use cached values from influence calcs? expand size to INTEREST_SIZE
    };

    class InterestMap
    {
      private:
        float Interest[INTEREST_SIZE] = { 0 };
        int2  WorldCenter;

        float InterestTemplates[2][INTEREST_SIZE] = { 0 };
        bool  InterestTemplateCreated[2]          = { false };

        float InfluenceCache[6][INTEREST_SIZE] = { 0 };
        bool  InfluenceCacheCreated[6]         = { false };

        Components::CreepProxIMAP   MyProxIMAP;
        Components::CreepThreatIMAP MyThreatIMAP;
        Core::FriendlyProxIMAP      FriendlyProxIMAP;
        Core::FriendlyThreatIMAP    FriendlyThreatIMAP;
        Core::EnemyProxIMAP         EnemyProxIMAP;
        Core::EnemyThreatIMAP       EnemyThreatIMAP;

      public:
        InterestMap(const CreepProxIMAP&            myProxIMAP,
                    const CreepThreatIMAP&          myThreatIMAP,
                    const Core::FriendlyProxIMAP&   friendlyProxIMAP,
                    const Core::FriendlyThreatIMAP& friendlyThreatIMAP,
                    const Core::EnemyProxIMAP&      enemyProxIMAP,
                    const Core::EnemyThreatIMAP&    enemyThreatIMAP):
            MyProxIMAP(myProxIMAP),
            MyThreatIMAP(myThreatIMAP),
            FriendlyProxIMAP(friendlyProxIMAP),
            FriendlyThreatIMAP(friendlyThreatIMAP),
            EnemyProxIMAP(enemyProxIMAP),
            EnemyThreatIMAP(enemyThreatIMAP)
        {
        }

        void Clear()
        {
            memset(Interest, 0, sizeof(float) * INTEREST_SIZE);
        }

        // TODO potentially add center only options?

        void Add(InfluenceType influenceType, float multiplier)
        {
            float* influenceTemplate = GetInfluenceCache(influenceType);
            for (int i = 0; i < INTEREST_SIZE; i++)
            {
                float value = Interest[i];
                value += influenceTemplate[i] + multiplier;
                Interest[i] = value;
            }
        }

        void Multiply(InfluenceType influenceType, float multiplier)
        {
            float* influenceTemplate = GetInfluenceCache(influenceType);
            for (int i = 0; i < INTEREST_SIZE; i++)
            {
                float value = Interest[i];
                value += influenceTemplate[i] * multiplier;
                Interest[i] = value;
            }
        }

        void Normalise()
        {
            float max = std::numeric_limits<float>::min();
            float min = std::numeric_limits<float>::max();

            for (int i = 0; i < INTEREST_SIZE; i++)
            {
                float value = Interest[i];
                if (value > max)
                    max = value;
                if (value < min)
                    min = value;
            }

            if (min == max)
            {
                float fixedVal = min <= 0 ? 0 : 1;
                for (int i = 0; i < INTEREST_SIZE; i++)
                {
                    Interest[i] = fixedVal;
                }
            }
            else
            {
                for (int i = 0; i < INTEREST_SIZE; i++)
                {
                    float value = Interest[i];
                    Interest[i] = (value - min) / (max - min);
                }
            }
        }

        void NormaliseAndInvert()
        {
            float max = std::numeric_limits<float>::min();
            float min = std::numeric_limits<float>::max();

            for (int i = 0; i < INTEREST_SIZE; i++)
            {
                float value = Interest[i];
                if (value > max)
                    max = value;
                if (value < min)
                    min = value;
            }

            if (min == max)
            {
                float fixedVal = min <= 0 ? 1 : 0;

                for (int i = 0; i < INTEREST_SIZE; i++)
                {
                    Interest[i] = fixedVal;
                }
            }
            else
            {
                for (int i = 0; i < INTEREST_SIZE; i++)
                {
                    float value = Interest[i];
                    Interest[i] = 1.0f - (value - min) / (max - min);
                }
            }
        }

        void ApplyInterestTemplate(InterestType interestType, int radius)
        {
            float* interestTemplate = GetInterestTemplate(interestType, radius);
            for (int i = 0; i < INTEREST_SIZE; i++)
            {
                Interest[i]  *= interestTemplate[i];
            }
        }

        int2 GetHighestPos()
        {
        }

        float GetCenter()
        {
        }

      private:
        float* GetInfluenceCache(InfluenceType influenceType)
        {
            if (!InfluenceCacheCreated[(int)influenceType])
            {
                if (influenceType == InfluenceType::MyProx || influenceType == InfluenceType::MyThreat)
                    CreateMyInfluenceCache(influenceType);
                else
                    CreateInfluenceCache(influenceType);
            }
            return InfluenceCache[(int)influenceType];
        }

        void CreateMyInfluenceCache(InfluenceType influenceType)
        {
            float* influence = InfluenceCache[(int)influenceType];
            // todo for x,y loop, using personal offsets
        }

        void CreateInfluenceCache(InfluenceType influenceType)
        {
            float* influence = InfluenceCache[(int)influenceType];
            // todo for x,y loop using map offsets
        }

        float* GetInterestTemplate(InterestType interestType)
        {
            if (!InterestTemplateCreated[(int)interestType])
                CreateInterestTemplate(interestType);
            return InterestTemplates[(int)interestType];
        }

        void CreateInterestTemplate(InterestType interestType)
        {
            float* interestTemplate = InfluenceCache[(int)interestType];
        }
    };
} // namespace Overseer::Systems::AI
#endif // OVERSEER_WASM_SRC_SYSTEMS_AI_INTERESTMAP_H_
