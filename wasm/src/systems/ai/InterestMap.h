//
// Created by Tim on 30/12/2021.
//

#ifndef OVERSEER_WASM_SRC_SYSTEMS_AI_INTERESTMAP_H_
#define OVERSEER_WASM_SRC_SYSTEMS_AI_INTERESTMAP_H_
#include "Includes.h"
#include "core/Math.h"
#include "core/Influence.h"
#include "components/Components.h"
#include "core/Structures.h"

namespace Overseer::Systems::AI
{
    enum struct InfluenceType
    {
        MyProx,
        MyThreat,
        FriendlyProx,
        FriendlyThreat,
        EnemyProx,
        EnemyThreat,
        SquadLeaderMovementMap
    };

    enum struct InterestType
    {
        Proximity,
        MovementMap
    };

    enum struct InterestRange
    {
        Range1 = 1,
        Range2 = 2,
        Range3 = 3,
        Range4 = 4,
        Range5 = 5,
        Range6 = 6,
        Range7 = 7,
        Range8 = 8
    };

    // TODO maybe store these on the stack instead?
    struct InterestTemplate
    {
        float* Influence;
        bool   Created = false;

        ~InterestTemplate()
        {
            if (Created)
                delete[] Influence;
        }
    };

    struct InfluenceCache
    {
        float* Influence;
        bool   Created = false;

        ~InfluenceCache()
        {
            if (Created)
                delete[] Influence;
        }
    };

    class InterestMap : public Core::LocalMap
    {
      public:
        float Interest[INTEREST_SIZE] = { 0 };

      private:
        InterestTemplate InterestTemplates[2][8];
        InfluenceCache   InfluenceCaches[6];

        CreepProxIMAP            MyProxIMAP;
        CreepThreatIMAP          MyThreatIMAP;
        CreepMovementMap         MyMovementMap;
        CreepMovementMap         SquadLeaderMovementMap;
        Core::FriendlyProxIMAP   FriendlyProxIMAP;
        Core::FriendlyThreatIMAP FriendlyThreatIMAP;
        Core::EnemyProxIMAP      EnemyProxIMAP;
        Core::EnemyThreatIMAP    EnemyThreatIMAP;
        Core::NavMap             NavMap;

      public:
        InterestMap(int2                            center,
                    const CreepProxIMAP&            myProxIMAP,
                    const CreepThreatIMAP&          myThreatIMAP,
                    const CreepMovementMap&         myMovementMap,
                    const CreepMovementMap&         squadLeaderMovementMap,
                    const Core::FriendlyProxIMAP&   friendlyProxIMAP,
                    const Core::FriendlyThreatIMAP& friendlyThreatIMAP,
                    const Core::EnemyProxIMAP&      enemyProxIMAP,
                    const Core::EnemyThreatIMAP&    enemyThreatIMAP,
                    const Core::NavMap&             navMap):
            MyProxIMAP(myProxIMAP),
            MyThreatIMAP(myThreatIMAP),
            MyMovementMap(myMovementMap),
            SquadLeaderMovementMap(squadLeaderMovementMap),
            FriendlyProxIMAP(friendlyProxIMAP),
            FriendlyThreatIMAP(friendlyThreatIMAP),
            EnemyProxIMAP(enemyProxIMAP),
            EnemyThreatIMAP(enemyThreatIMAP),
            NavMap(navMap)
        {
            ClampAndSetBounds(center, INTEREST_RADIUS, INTEREST_WIDTH);
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
                value       = influenceCache.Influence[i] * multiplier + value;
                Interest[i] = value;
            }
        }

        void Multiply(InfluenceType influenceType, float multiplier)
        {
            InfluenceCache& influenceCache = GetInfluenceCache(influenceType);
            for (int i = 0; i < INTEREST_SIZE; i++)
            {
                float value = Interest[i];
                value       = influenceCache.Influence[i] * multiplier * value;
                Interest[i] = value;
            }
        }

        void Clamp(float minimum, float maximum)
        {
            for (int i = 0; i < INTEREST_SIZE; i++)
            {
                Interest[i] = clamp(Interest[i], minimum, maximum);
            }
        }

        void ClampToZero()
        {
            for (int i = 0; i < INTEREST_SIZE; i++)
            {
                Interest[i] = max(Interest[i], 0.0f);
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

        void ApplyInterestTemplate(InterestType interestType)
        {
            InterestTemplate& interestTemplate = GetInterestTemplate(interestType, (InterestRange)INTEREST_RADIUS);
            for (int i = 0; i < INTEREST_SIZE; i++)
            {
                Interest[i] *= interestTemplate.Influence[i];
            }
        }

        void ApplyInterestTemplate(InterestType interestType, InterestRange range)
        {
            InterestTemplate& interestTemplate = GetInterestTemplate(interestType, range);
            for (int i = 0; i < INTEREST_SIZE; i++)
            {
                Interest[i] *= interestTemplate.Influence[i];
            }
        }

        int2 GetHighestPos()
        {
            // TODO re introduce random
            int   offset   = (rand() % INTEREST_SIZE);
            // printf("Offset: %i\n", offset);
            float max      = std::numeric_limits<float>::min();
            int   maxIndex = -1;

            int worldIndex = WorldStartIndex;
            int localIndex = LocalStartIndex;

            int width           = LocalEnd.x - LocalStart.x;
            int worldYIncrement = MAP_WIDTH - width;
            int localYIncrement = INTEREST_WIDTH - width;

            // printf("GetHighestPos: { ");
            for (int y = LocalStart.y; y < LocalEnd.y; ++y)
            {
                for (int x = LocalStart.x; x < LocalEnd.x; ++x)
                {

                    float value = Interest[localIndex];
                    // printf("{ WorldIndex: %i, LocalIndex: %i, Value: %f }, ", worldIndex, localIndex, value);
                    if (value > max) // TODO WHEN SETTING THIS TO <= WE FAIL ON THE PATHFINDING, IT SHOULD STILL WORK AS RANGES ARE THE SAME, IT CHOOSES BOTTOM LEFT POSITION AND NO PATH CAN BE FOUND, INVESTIGATE
                    {
                        max      = value;
                        maxIndex = worldIndex;
                    }

                    worldIndex++;
                    localIndex++;
                }
                worldIndex += worldYIncrement;
                localIndex += localYIncrement;
            }
            // printf("} \n");

            // // printf("{ ");
            // for (int i = offset; i < INTEREST_SIZE; ++i)
            // {
            //     float value = Interest[i];
            //     // printf("{ Index: %i, Value: %f }, ", i, value);
            //     if (value <= max)
            //         continue;
            //     max      = value;
            //     maxIndex = i;
            // }
            // // printf(" }\n");
            //
            // // printf("{ ");
            // for (int i = 0; i < offset; ++i)
            // {
            //     float value = Interest[i];
            //     // printf("{Index: %i, Value: %f }, ", i, value);
            //     if (value <= max)
            //         continue;
            //     max      = value;
            //     maxIndex = i;
            // }
            // // printf(" }\n");

            int2 worldPos =  IndexToPos(maxIndex, MAP_WIDTH);

            // printf("HIGHEST VAL: %f\n", max);

            return worldPos;
        }

        float GetCenter()
        {
            return Interest[LocalCenterIndex];
        }

      private:
        InfluenceCache& GetInfluenceCache(InfluenceType influenceType)
        {
            InfluenceCache& cache = InfluenceCaches[(int)influenceType];
            if (!cache.Created)
                InfluenceCaches[(int)influenceType] = CreateInfluenceCache(influenceType, cache);
            return cache;
        }

        InfluenceCache& CreateInfluenceCache(InfluenceType influenceType, InfluenceCache& cache)
        {
            cache.Created = true;
            switch (influenceType)
            {
                case InfluenceType::MyProx:
                    cache.Influence = new float[INTEREST_SIZE] { 0 };
                    CopyFromProxMap(MyProxIMAP, cache.Influence);
                    break;
                case InfluenceType::MyThreat:
                    cache.Influence = new float[INTEREST_SIZE] { 0 };
                    CopyFromThreatMap(MyThreatIMAP, cache.Influence);
                    break;
                case InfluenceType::FriendlyProx:
                    cache.Influence = new float[INTEREST_SIZE];
                    CopyFromIMAP(FriendlyProxIMAP, cache.Influence);
                    break;
                case InfluenceType::FriendlyThreat:
                    cache.Influence = new float[INTEREST_SIZE];
                    CopyFromIMAP(FriendlyThreatIMAP, cache.Influence);
                    break;
                case InfluenceType::EnemyProx:
                    cache.Influence = new float[INTEREST_SIZE];
                    CopyFromIMAP(EnemyProxIMAP, cache.Influence);
                    break;
                case InfluenceType::EnemyThreat:
                    cache.Influence = new float[INTEREST_SIZE];
                    CopyFromIMAP(EnemyThreatIMAP, cache.Influence);
                    break;
                case InfluenceType::SquadLeaderMovementMap:
                    cache.Influence = new float[INTEREST_SIZE] { 0 };
                    CopyFromMovementMap(SquadLeaderMovementMap, cache.Influence);
                    break;
            }

            return cache;
        }

        void CopyFromIMAP(Core::IMAP& imap, float* influence)
        {
            int worldIndex = WorldStartIndex;
            int localIndex = LocalStartIndex;

            int width           = LocalEnd.x - LocalStart.x;
            int worldYIncrement = MAP_WIDTH - width;
            int localYIncrement = INTEREST_WIDTH - width;

            // printf("CopyFromIMAP: { ");
            for (int y = LocalStart.y; y < LocalEnd.y; ++y)
            {
                for (int x = LocalStart.x; x < LocalEnd.x; ++x)
                {
                    float inf             = imap.Influence[worldIndex];
                    influence[localIndex] = inf;
                    // printf("Inf: %f, ", inf);
                    worldIndex++;
                    localIndex++;
                }
                worldIndex += worldYIncrement;
                localIndex += localYIncrement;
            }
            // printf("} \n");
        }

        void CopyFromProxMap(CreepProxIMAP& proxIMAP, float* influence)
        {
            int interestIndex = PosToIndex(proxIMAP.WorldStart - WorldStart, INTEREST_WIDTH) + LocalStartIndex;
            int proxIndex     = proxIMAP.LocalStartIndex;

            int width              = proxIMAP.LocalEnd.x - proxIMAP.LocalStart.x;
            int interestYIncrement = INTEREST_WIDTH - width;
            int proxYIncrement     = INFLUENCE_PROX_WIDTH - width;

            // printf("CopyFromProxMap: { ");
            for (int y = proxIMAP.LocalStart.y; y < proxIMAP.LocalEnd.y; ++y)
            {
                for (int x = proxIMAP.LocalStart.x; x < proxIMAP.LocalEnd.x; ++x)
                {
                    float inf                = proxIMAP.Influence[proxIndex];
                    influence[interestIndex] = inf;
                    // printf("Inf: %f, ", inf);
                    interestIndex++;
                    proxIndex++;
                }
                interestIndex += interestYIncrement;
                proxIndex += proxYIncrement;
            }
            // printf("} \n");
        };

        void CopyFromThreatMap(CreepThreatIMAP& threatIMAP, float* influence)
        {
            int interestIndex = PosToIndex(threatIMAP.WorldStart - WorldStart, INTEREST_WIDTH) + LocalStartIndex;
            int threatIndex   = threatIMAP.LocalStartIndex;

            int width              = threatIMAP.LocalEnd.x - threatIMAP.LocalStart.x;
            int interestYIncrement = INTEREST_WIDTH - width;
            int threatYIncrement   = INFLUENCE_THREAT_WIDTH - width;

            // printf("CopyFromThreatMap: { ");
            for (int y = threatIMAP.LocalStart.y; y < threatIMAP.LocalEnd.y; ++y)
            {
                for (int x = threatIMAP.LocalStart.x; x < threatIMAP.LocalEnd.x; ++x)
                {
                    float inf                = threatIMAP.Influence[threatIndex];
                    influence[interestIndex] = inf;
                    // printf("Inf: %f, ", inf);
                    interestIndex++;
                    threatIndex++;
                }
                interestIndex += interestYIncrement;
                threatIndex += threatYIncrement;
            }
            // printf("} \n");
        };

        void CopyFromMovementMap(CreepMovementMap& movementMap, float* influence)
        {
            // printf("CopyFromMovementMap: { ");
            for (int i = 0; i < INTEREST_SIZE; ++i)
            {
                int cost     = movementMap.Nodes[i].Cost;
                // Turns 0 -> distance into 1 -> 0
                float inf = CalculateInverseLinearInfluence(1.0f, cost, INTEREST_WIDTH);
                // printf("{ LocalIndex: %i, Inf: %f }, ", i, inf);
                influence[i] = inf;
            }
            // printf("} \n");
        }

        InterestTemplate& GetInterestTemplate(InterestType interestType, InterestRange range)
        {
            InterestTemplate& temp = InterestTemplates[(int)interestType][(int)range - 1];
            if (!temp.Created)
                InterestTemplates[(int)interestType][(int)range - 1] = CreateInterestTemplate(interestType, range, temp);
            return temp;
        }

        InterestTemplate& CreateInterestTemplate(InterestType interestType, InterestRange range, InterestTemplate& temp)
        {
            temp.Created   = true;
            temp.Influence = new float[INTEREST_SIZE];

            switch (interestType)
            {
                case InterestType::Proximity:
                    CreateProximityTemplate(temp.Influence, range);
                    break;
                case InterestType::MovementMap:
                    if ((int)range == INTEREST_RADIUS)
                        CopyFromMovementMap(MyMovementMap, temp.Influence);
                    else
                        CreateMovementMapTemplate(MyMovementMap, temp.Influence, range);
                    break;
            }

            return temp;
        }

        void CreateProximityTemplate(float* influence, InterestRange range)
        {
            int worldIndex = WorldStartIndex;
            int localIndex = LocalStartIndex;

            int width           = LocalEnd.x - LocalStart.x;
            int worldYIncrement = MAP_WIDTH - width;
            int localYIncrement = INTEREST_WIDTH - width;

            int rangeInt = (int)range;

            // printf("CreateProximityTemplate: { ");
            for (int y = LocalStart.y; y < LocalEnd.y; ++y)
            {
                for (int x = LocalStart.x; x < LocalEnd.x; ++x)
                {
                    int dist              = DistanceChebyshev(LocalCenter, int2(x, y));
                    dist                  = NavMap.Map[worldIndex] == INT_MAXVALUE ? rangeInt : dist;
                    float inf             = CalculateInverseLinearInfluence(1.0f, dist, rangeInt);
                    influence[localIndex] = inf;
                    // printf("{ WorldIndex: %i, LocalIndex: %i, Inf: %f }, ", worldIndex, localIndex, inf);
                    worldIndex++;
                    localIndex++;
                }
                worldIndex += worldYIncrement;
                localIndex += localYIncrement;
            }
            // printf("} \n");
        }

        void CreateMovementMapTemplate(CreepMovementMap& movementMap, float* influence, InterestRange range)
        {
            float maxDist = (float)range;
            for (int i = 0; i < INTEREST_SIZE; ++i)
            {
                int cost     = movementMap.Nodes[i].Cost;
                // Turns 0 -> distance into 1 -> 0
                influence[i] = CalculateInverseLinearInfluence(1.0f, cost, maxDist);
            }
        }
    };
} // namespace Overseer::Systems::AI
#endif // OVERSEER_WASM_SRC_SYSTEMS_AI_INTERESTMAP_H_
