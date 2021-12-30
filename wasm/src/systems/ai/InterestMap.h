//
// Created by Tim on 30/12/2021.
//

#ifndef OVERSEER_WASM_SRC_SYSTEMS_AI_INTERESTMAP_H_
#define OVERSEER_WASM_SRC_SYSTEMS_AI_INTERESTMAP_H_
#include "Includes.h"
#include "core/Math.h"
#include "core/Influence.h"
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

    struct InterestTemplate : Core::LocalMap
    {
        float* Influence = new float[INTEREST_SIZE] { 0 };
        bool   Created   = false;
    };

    struct InfluenceCache : Core::LocalMap
    {
        float* Influence = new float[INTEREST_SIZE] { 0 };
        bool   Created   = false;
    };

    class InterestMap
    {
      private:
        float Interest[INTEREST_SIZE] = { 0 };
        int2  WorldCenter;

        InterestTemplate InterestTemplates[2];
        InfluenceCache   InfluenceCaches[6];

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
            InfluenceCache& influenceCache = GetInfluenceCache(influenceType);
            for (int i = 0; i < INTEREST_SIZE; i++)
            {
                float value = Interest[i];
                value += influenceCache.Influence[i] + multiplier;
                Interest[i] = value;
            }
        }

        void Multiply(InfluenceType influenceType, float multiplier)
        {
            InfluenceCache& influenceCache = GetInfluenceCache(influenceType);
            for (int i = 0; i < INTEREST_SIZE; i++)
            {
                float value = Interest[i];
                value += influenceCache.Influence[i] * multiplier;
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
            InterestTemplate& interestTemplate = GetInterestTemplate(interestType, radius);
            for (int i = 0; i < INTEREST_SIZE; i++)
            {
                Interest[i] *= interestTemplate.Influence[i];
            }
        }

        int2 GetHighestPos()
        {
        }

        float GetCenter()
        {
        }

      private:
        InfluenceCache& GetInfluenceCache(InfluenceType influenceType)
        {
            InfluenceCache& cache = InfluenceCaches[(int)influenceType];
            if (!cache.Created)
            {
                if (influenceType == InfluenceType::MyProx || influenceType == InfluenceType::MyThreat)
                    InfluenceCaches[(int)influenceType] = CreateMyInfluenceCache(influenceType, cache);
                else
                    InfluenceCaches[(int)influenceType] = CreateInfluenceCache(influenceType, cache);
            }
            return cache;
        }

        InfluenceCache& CreateMyInfluenceCache(InfluenceType influenceType, InfluenceCache& cache)
        {
            cache.Created    = true;
            float* influence = cache.Influence;
            // todo for x,y loop, using personal offsets
            return cache;
        }

        InfluenceCache& CreateInfluenceCache(InfluenceType influenceType, InfluenceCache& cache)
        {
            cache.Created    = true;
            float* influence = cache.Influence;
            // todo for x,y loop using map offsets
            return cache;
        }

        InterestTemplate& GetInterestTemplate(InterestType interestType, int radius)
        {
            InterestTemplate& temp = InterestTemplates[(int)interestType];
            if (!temp.Created)
                InterestTemplates[(int)interestType] = CreateInterestTemplate(interestType, temp);
            return temp;
        }

        InterestTemplate& CreateInterestTemplate(InterestType interestType, InterestTemplate& temp)
        {
            temp.Created     = true;
            float* influence = temp.Influence;
            return temp;
        }
    };
} // namespace Overseer::Systems::AI
#endif // OVERSEER_WASM_SRC_SYSTEMS_AI_INTERESTMAP_H_
