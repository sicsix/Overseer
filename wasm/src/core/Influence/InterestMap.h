//
// Created by Tim on 30/12/2021.
//

#ifndef OVERSEER_WASM_SRC_SYSTEMS_AI_INTERESTMAP_H_
#define OVERSEER_WASM_SRC_SYSTEMS_AI_INTERESTMAP_H_
#include "Includes.h"
#include "core/Math.h"
#include "components/Components.h"
#include "core/Structures.h"
#include "CreepProx.h"
#include "CreepThreat.h"
#include "ProxIMAP.h"
#include "ThreatIMAP.h"

namespace Overseer::Core::Influence
{
    enum struct InfluenceType
    {
        MyProx,
        MyThreat,
        FriendlyProx,
        FriendlyThreat,
        EnemyProx,
        EnemyThreat,
        LeaderMovement
    };

    enum struct InterestType
    {
        Proximity,
        Movement
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
        float Influence[INTEREST_SIZE] = {};
        bool  Created                  = false;

        // ~InterestTemplate()
        // {
        //     printf("FREE INTEREST\n");
        //     if (Created)
        //         delete[] Influence;
        // }
    };

    struct InfluenceCache
    {
        float Influence[INTEREST_SIZE] = {};
        bool  Created                  = false;

        // ~InfluenceCache()
        // {
        //     printf("FREE INFLUENCE\n");
        //     if (Created)
        //         delete[] Influence;
        // }
    };

    class InterestMap : public LocalMap
    {
      public:
        float Interest[INTEREST_SIZE] = { 0 };

      private:
        InterestTemplate InterestTemplates[2][8];
        InfluenceCache   InfluenceCaches[7];

        CreepProx          MyProx;
        CreepThreat        MyThreat;
        CreepMovement      MyMovement;
        CreepMovement      LeaderMovement;
        FriendlyProxIMAP   FriendlyProx;
        FriendlyThreatIMAP FriendlyThreat;
        EnemyProxIMAP      EnemyProx;
        EnemyThreatIMAP    EnemyThreat;
        Core::NavMap       NavMap;

      public:
        InterestMap(int2                             center,
                    const CreepProx&                 myProx,
                    const CreepThreat&               myThreat,
                    const CreepMovement&             myMovement,
                    const CreepMovement&             leaderMovement,
                    const struct FriendlyProxIMAP&   friendlyProx,
                    const struct FriendlyThreatIMAP& friendlyThreat,
                    const struct EnemyProxIMAP&      enemyProx,
                    const struct EnemyThreatIMAP&    enemyThreat,
                    const Core::NavMap&              navMap):
            MyProx(myProx),
            MyThreat(myThreat),
            MyMovement(myMovement),
            LeaderMovement(leaderMovement),
            FriendlyProx(friendlyProx),
            FriendlyThreat(friendlyThreat),
            EnemyProx(enemyProx),
            EnemyThreat(enemyThreat),
            NavMap(navMap)
        {
            ClampAndSetBounds(center, INTEREST_RADIUS, INTEREST_WIDTH);
        }

        void Clear()
        {
            memset(Interest, 0, sizeof(float) * INTEREST_SIZE);
        }

        // TODO potentially add center only options?

        void Add(float value)
        {
            for (int i = 0; i < INTEREST_SIZE; i++)
            {
                Interest[i] += value;
            }
        }

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
            float max = std::numeric_limits<float>::lowest();
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
            float max = std::numeric_limits<float>::lowest();
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

        void NormaliseAndInvert(float minVal, float maxVal)
        {
            float max = std::numeric_limits<float>::lowest();
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
                    Interest[i] = (maxVal - minVal) - (value - min) / (max - min) + minVal;
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
            // int   offset   = (rand() % INTEREST_SIZE);
            int   offset   = 0;
            // printf("Offset: %i\n", offset);
            float max      = std::numeric_limits<float>::lowest();
            int   maxIndex = -1;

            // int worldIndex = WorldStartIndex;
            // int localIndex = LocalStartIndex;
            //
            // int width           = LocalEnd.x - LocalStart.x;
            // int worldYIncrement = MAP_WIDTH - width;
            // int localYIncrement = INTEREST_WIDTH - width;
            //
            // // TODO dont need to do this, use lower one as faster, everything outside the map bounds should be zero anyway
            // // printf("GetHighestPos: { ");
            // for (int y = LocalStart.y; y < LocalEnd.y; ++y)
            // {
            //     for (int x = LocalStart.x; x < LocalEnd.x; ++x)
            //     {
            //         float value = Interest[localIndex];
            //         // printf("{ WorldIndex: %i, LocalIndex: %i, Value: %f }, ", worldIndex, localIndex, value);
            //         if (value > max)
            //         {
            //             max      = value;
            //             maxIndex = worldIndex;
            //         }
            //
            //         worldIndex++;
            //         localIndex++;
            //     }
            //     worldIndex += worldYIncrement;
            //     localIndex += localYIncrement;
            // }
            // // printf("} \n");

            // printf("{ ");
            for (int i = offset; i < INTEREST_SIZE; ++i)
            {
                float value = Interest[i];
                // printf("{ Index: %i, Value: %f }, ", i, value);
                if (value <= max)
                    continue;
                max      = value;
                maxIndex = i;
            }
            // printf(" }\n");

            // printf("{ ");
            for (int i = 0; i < offset; ++i)
            {
                float value = Interest[i];
                // printf("{Index: %i, Value: %f }, ", i, value);
                if (value <= max)
                    continue;
                max      = value;
                maxIndex = i;
            }
            // printf(" }\n");

            int2 localOffset = IndexToPos(maxIndex - LocalStartIndex, INTEREST_WIDTH);
            int2 worldPos    = WorldStart + localOffset;
            // int2 worldPos = IndexToPos(maxIndex, MAP_WIDTH);

            // printf(
            //     "MAXINDEX: %i    LOCAL POS: { %i, %i }    HIGHEST POS: { %i, %i }    HIGHEST VAL: %f    LOCALSTARTINDEX: %i    LOCALOFFSET: { %i, %i }\n",
            //     maxIndex,
            //     localOffset.x,
            //     localOffset.y,
            //     worldPos.x,
            //     worldPos.y,
            //     max,
            //     LocalStartIndex,
            //     localOffset.x,
            //     localOffset.y);

            // if (maxIndex == -1)
            // {
            //     printf("INTEREST RETURNED -1 : { ");
            //     for (int i = 0; i < INTEREST_SIZE; ++i)
            //     {
            //         float value = Interest[i];
            //         printf("{ Idx: %i, Int: %f }, ", i, value);
            //     }
            //     printf("}\n");
            // }

            worldPos = max <= 0.0f ? int2(-1, -1) : worldPos;

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
                    // cache.Influence = new float[INTEREST_SIZE] { 0 };
                    CopyFromProxMap(MyProx, cache.Influence);
                    break;
                case InfluenceType::MyThreat:
                    // cache.Influence = new float[INTEREST_SIZE] { 0 };
                    CopyFromThreatMap(MyThreat, cache.Influence);
                    break;
                case InfluenceType::FriendlyProx:
                    // cache.Influence = new float[INTEREST_SIZE];
                    CopyFromIMAP(FriendlyProx, cache.Influence);
                    break;
                case InfluenceType::FriendlyThreat:
                    // cache.Influence = new float[INTEREST_SIZE];
                    CopyFromIMAP(FriendlyThreat, cache.Influence);
                    break;
                case InfluenceType::EnemyProx:
                    // cache.Influence = new float[INTEREST_SIZE];
                    CopyFromIMAP(EnemyProx, cache.Influence);
                    break;
                case InfluenceType::EnemyThreat:
                    // cache.Influence = new float[INTEREST_SIZE];
                    CopyFromIMAP(EnemyThreat, cache.Influence);
                    break;
                case InfluenceType::LeaderMovement:
                    // cache.Influence = new float[INTEREST_SIZE] { 0 };
                    CopyFromMovementMap(LeaderMovement, cache.Influence);
                    break;
            }

            return cache;
        }

        void CopyFromIMAP(IMAP& imap, float* influence)
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
#ifdef DEBUG_ENABLED
                    if (worldIndex < 0 || worldIndex >= MAP_SIZE || localIndex < 0 || localIndex >= INTEREST_SIZE)
                        printf("[WASM] ERROR: InterestMap::CopyFromIMAP - OUT OF RANGE\n");
#endif
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

        void CopyFromProxMap(CreepProx& prox, float* influence)
        {
            // NOTE MyProxMap is always centered on same location and contained within the interest map

            int interestIndex = PosToIndex(prox.WorldStart - WorldStart, INTEREST_WIDTH) + LocalStartIndex;
            int proxIndex     = prox.LocalStartIndex;

            int width              = prox.LocalEnd.x - prox.LocalStart.x;
            int interestYIncrement = INTEREST_WIDTH - width;
            int proxYIncrement     = INFLUENCE_PROX_WIDTH - width;

            // printf("CopyFromProxMap: { ");
            for (int y = prox.LocalStart.y; y < prox.LocalEnd.y; ++y)
            {
                for (int x = prox.LocalStart.x; x < prox.LocalEnd.x; ++x)
                {
#ifdef DEBUG_ENABLED
                    if (interestIndex < 0 || interestIndex >= INTEREST_SIZE || proxIndex < 0 ||
                        proxIndex >= INFLUENCE_PROX_SIZE)
                        printf("[WASM] ERROR: InterestMap::CopyFromProxMap - OUT OF RANGE\n");
#endif
                    float inf                = prox.Influence[proxIndex];
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

        void CopyFromThreatMap(CreepThreat& threat, float* influence)
        {
            // NOTE MyThreatMap is always centered on same location and contained within the interest map
            int interestIndex = PosToIndex(threat.WorldStart - WorldStart, INTEREST_WIDTH) + LocalStartIndex;
            int threatIndex   = threat.LocalStartIndex;

            int width              = threat.LocalEnd.x - threat.LocalStart.x;
            int interestYIncrement = INTEREST_WIDTH - width;
            int threatYIncrement   = INFLUENCE_THREAT_WIDTH - width;

            // printf("CopyFromThreatMap: { ");
            for (int y = threat.LocalStart.y; y < threat.LocalEnd.y; ++y)
            {
                for (int x = threat.LocalStart.x; x < threat.LocalEnd.x; ++x)
                {
#ifdef DEBUG_ENABLED
                    if (interestIndex < 0 || interestIndex >= INTEREST_SIZE || threatIndex < 0 ||
                        threatIndex >= INFLUENCE_THREAT_SIZE)
                        printf("[WASM] ERROR: InterestMap::CopyFromThreatMap - OUT OF RANGE\n");
#endif
                    float inf                = threat.Influence[threatIndex];
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

        void CopyFromMovementMap(CreepMovement& movement, float* influence)
        {
            int2 worldStart = max(movement.WorldStart, WorldStart);
            int2 worldEnd   = min(movement.WorldEnd, WorldEnd);

            int interestIndex = PosToIndex(worldStart - WorldStart, INTEREST_WIDTH) + LocalStartIndex;
            int movementIndex = PosToIndex(worldStart - movement.WorldStart, INTEREST_WIDTH) + movement.LocalStartIndex;

            int width      = worldEnd.x - worldStart.x;
            int yIncrement = INTEREST_WIDTH - width;

            // printf(
            //     "worldStart: { %i, %i }    worldEnd: { %i, %i }    interestIndex: %i    movementIndex: %i    width: %i    yIncrement: %i",
            //     worldStart.x,
            //     worldStart.y,
            //     worldEnd.x,
            //     worldEnd.y,
            //     interestIndex,
            //     movementIndex,
            //     width,
            //     yIncrement);

            // printf("CopyFromMovementMap: { ");
            for (int y = worldStart.y; y < worldEnd.y; ++y)
            {
                for (int x = worldStart.x; x < worldEnd.x; ++x)
                {
#ifdef DEBUG_ENABLED
                    if (interestIndex < 0 || interestIndex >= INTEREST_SIZE || movementIndex < 0 ||
                        movementIndex >= INTEREST_SIZE)
                        printf("[WASM] ERROR: InterestMap::CopyFromMovementMap - OUT OF RANGE\n");
#endif
                    int   cost = movement.Nodes[movementIndex].Cost;
                    float inf  = CalculateInverseLinearInfluence(1.0f, cost, INTEREST_RADIUS + 1);
                    // printf("{ InterestIndex: %i, MovementIndex: %i, Inf: %f }, ", interestIndex, movementIndex, inf);

                    influence[interestIndex] = inf;
                    interestIndex++;
                    movementIndex++;
                }
                interestIndex += yIncrement;
                movementIndex += yIncrement;
            }
            // printf("} \n");
        }

        InterestTemplate& GetInterestTemplate(InterestType interestType, InterestRange range)
        {
            InterestTemplate& temp = InterestTemplates[(int)interestType][(int)range - 1];
            if (!temp.Created)
                InterestTemplates[(int)interestType][(int)range - 1] =
                    CreateInterestTemplate(interestType, range, temp);
            return temp;
        }

        InterestTemplate& CreateInterestTemplate(InterestType interestType, InterestRange range, InterestTemplate& temp)
        {
            temp.Created = true;
            // temp.Influence = new float[INTEREST_SIZE];

            switch (interestType)
            {
                case InterestType::Proximity:
                    CreateProximityTemplate(temp.Influence, range);
                    break;
                case InterestType::Movement:
                    if ((int)range == INTEREST_RADIUS)
                        CreateMovementTemplate(MyMovement, temp.Influence);
                    else
                        CreateMovementTemplate(MyMovement, temp.Influence, range);
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
#ifdef DEBUG_ENABLED
                    if (worldIndex < 0 || worldIndex >= MAP_SIZE || localIndex < 0 || localIndex >= INTEREST_SIZE)
                        printf("[WASM] ERROR: InterestMap::CreateProximityTemplate - OUT OF RANGE\n");
#endif
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

        void CreateMovementTemplate(CreepMovement& movement, float* influence)
        {
            for (int i = 0; i < INTEREST_SIZE; ++i)
            {
                int   cost   = movement.Nodes[i].Cost;
                float inf    = CalculateInverseLinearInfluence(1.0f, cost, INTEREST_RADIUS + 1);
                // Turns 0 -> distance into 1 -> 0
                // printf("%f", inf);
                influence[i] = inf;
            }
        }

        void CreateMovementTemplate(CreepMovement& movement, float* influence, InterestRange range)
        {
            float maxDist = (float)range;
            for (int i = 0; i < INTEREST_SIZE; ++i)
            {
                int cost     = movement.Nodes[i].Cost;
                // Turns 0 -> distance into 1 -> 0
                influence[i] = CalculateInverseLinearInfluence(1.0f, cost, maxDist + 1);
            }
        }

      public:
        void DebugDraw(float minVal, int maxCount)
        {
            int worldIndex = WorldStartIndex;
            int localIndex = LocalStartIndex;

            int width           = LocalEnd.x - LocalStart.x;
            int worldYIncrement = MAP_WIDTH - width;
            int localYIncrement = INTEREST_WIDTH - width;

            int count = 0;
            for (int y = LocalStart.y; y < LocalEnd.y; ++y)
            {
                for (int x = LocalStart.x; x < LocalEnd.x; ++x)
                {
                    float interest = Interest[localIndex];
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
} // namespace Overseer::Core::Influence
#endif // OVERSEER_WASM_SRC_SYSTEMS_AI_INTERESTMAP_H_
