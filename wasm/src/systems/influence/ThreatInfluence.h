//
// Created by Tim on 30/12/2021.
//

#ifndef OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_THREATINFLUENCE_H_
#define OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_THREATINFLUENCE_H_
#include "components/Components.h"
#include "core/MovementCoster.h"
#include "core/LineOfSight.h"

namespace Overseer::Systems::Influence
{
    struct ThreatInfluence
    {
      public:
        static void Calculate(CreepThreatIMAP&   threatIMAP,
                              Core::LineOfSight& lineOfSight,
                              Threat&            threat,
                              CreepMovementMap&  creepMovementMap)
        {
            // TODO threat should include hitpoints
            // Attack threat should also depend on ability to move, with no move ability the radius can be 1
            // Is heal a threat? or just an indirect threat? SHould it be included in threat influence?
            if (threat.AttackDamage > 0)
                SetAttackThreat(threatIMAP, threat, creepMovementMap);
            else
                memset(threatIMAP.Influence, 0, sizeof(float) * INFLUENCE_THREAT_SIZE);

            if (threat.RangedAttackDamage > 0 || threat.RangedHealRate > 0)
            {
                int los[INFLUENCE_THREAT_SIZE];
                lineOfSight.CalculateLOS(threatIMAP, los);

                if (threat.RangedAttackDamage > 0)
                    AddRangedAttackThreat(threatIMAP, threat, los);

                if (threat.RangedHealRate > 0)
                    AddRangedHealThreat(threatIMAP, threat, los);
            }
        }

      private:
        static void SetAttackThreat(CreepThreatIMAP& threatIMAP, Threat& threat, CreepMovementMap& creepMovementMap)
        {
            int localIndex = threatIMAP.LocalStartIndex;
            int movIndex = PosToIndex(WorldToLocal(threatIMAP.WorldStart, creepMovementMap.WorldStart), INTEREST_WIDTH);
            int width    = threatIMAP.LocalEnd.x - threatIMAP.LocalStart.x;
            int localYIncrement = INFLUENCE_THREAT_WIDTH - width;
            int movYIncrement   = INTEREST_WIDTH - width;

            // printf(
            //     "THREAT - WorldStart: { %i, %i }    WorldEnd: { %i, %i }    WorldCenter: { %i, %i }    WorldStartIndex: %i    LocalStart: { %i, %i }    LocalEnd: { %i, %i }    LocalStartIndex: %i    LocalYIncrement: %i\n",
            //     threatIMAP.WorldStart.x,
            //     threatIMAP.WorldStart.y,
            //     threatIMAP.WorldEnd.x,
            //     threatIMAP.WorldEnd.y,
            //     threatIMAP.WorldCenter.x,
            //     threatIMAP.WorldCenter.y,
            //     threatIMAP.WorldStartIndex,
            //     threatIMAP.LocalStart.x,
            //     threatIMAP.LocalStart.y,
            //     threatIMAP.LocalEnd.x,
            //     threatIMAP.LocalEnd.y,
            //     threatIMAP.LocalStartIndex,
            //     localYIncrement);
            //
            // printf(
            //     "MOVE - WorldStart: { %i, %i }    WorldEnd: { %i, %i }    WorldCenter: { %i, %i }    WorldStartIndex: %i    LocalStart: { %i, %i }    LocalEnd: { %i, %i }    LocalStartIndex: %i    LocalYIncrement: %i    AdjustedStart: %i\n",
            //     creepMovementMap.WorldStart.x,
            //     creepMovementMap.WorldStart.y,
            //     creepMovementMap.WorldEnd.x,
            //     creepMovementMap.WorldEnd.y,
            //     creepMovementMap.WorldCenter.x,
            //     creepMovementMap.WorldCenter.y,
            //     creepMovementMap.WorldStartIndex,
            //     creepMovementMap.LocalStart.x,
            //     creepMovementMap.LocalStart.y,
            //     creepMovementMap.LocalEnd.x,
            //     creepMovementMap.LocalEnd.y,
            //     creepMovementMap.LocalStartIndex,
            //     movYIncrement,
            //     movIndex);

            // printf("CalculateAttackThreatInfluence: { ");
            for (int y = threatIMAP.LocalStart.y; y < threatIMAP.LocalEnd.y; ++y)
            {
                for (int x = threatIMAP.LocalStart.x; x < threatIMAP.LocalEnd.x; ++x)
                {
                    int   moveCost = creepMovementMap.Nodes[movIndex].Cost * threat.TicksPerMove;
                    float distBase = CalculateInverseLinearInfluence(1.0f, moveCost, INFLUENCE_THREAT_RADIUS);
                    float inf      = distBase * threat.AttackDamage * INFLUENCE_THREAT_ATTACK_FACTOR;
                    threatIMAP.Influence[localIndex] = inf;
                    // printf("{ LocalIndex: %i, MoveCost: %i, DistBase: %f, Inf: %f }, ",
                    //        localIndex,
                    //        moveCost,
                    //        distBase,
                    //        inf);
                    localIndex++;
                    movIndex++;
                }
                localIndex += localYIncrement;
                movIndex += movYIncrement;
            }
            // printf("} \n");
        }

        static void AddRangedAttackThreat(CreepThreatIMAP& threatIMAP, Threat& threat, int* los)
        {
            int localIndex      = threatIMAP.LocalStartIndex;
            int width           = threatIMAP.LocalEnd.x - threatIMAP.LocalStart.x;
            int localYIncrement = INFLUENCE_THREAT_WIDTH - width;

            // TODO handle non moving creeps

            // printf("CalculateRangedAttackThreatInfluence: { ");
            for (int y = threatIMAP.LocalStart.y; y < threatIMAP.LocalEnd.y; ++y)
            {
                for (int x = threatIMAP.LocalStart.x; x < threatIMAP.LocalEnd.x; ++x)
                {
                    int   inLos      = los[localIndex];
                    float threatBase = inLos ? InfluencePrecomputes::ThreatRangedAttack4Falloff[localIndex] : 0;
                    float inf        = threatBase * threat.RangedAttackDamage * INFLUENCE_THREAT_RANGED_ATTACK_FACTOR;
                    threatIMAP.Influence[localIndex] += inf;
                    // if (inf > 0)
                    // {
                    //     // printf("{ MapIndex: %i, InfIndex: %i, InLos: %i, ThreatBase: %f, Inf: %f }, ",
                    //            mapIndex,
                    //            infIndex,
                    //            inLos,
                    //            threatBase,
                    //            inf);
                    // }
                    localIndex++;
                }
                localIndex += localYIncrement;
            }
            // printf("} \n");
        }

        static void AddRangedHealThreat(CreepThreatIMAP& threatIMAP, Threat& threat, int* los)
        {
            int localIndex      = threatIMAP.LocalStartIndex;
            int width           = threatIMAP.LocalEnd.x - threatIMAP.LocalStart.x;
            int localYIncrement = INFLUENCE_THREAT_WIDTH - width;

            // TODO handle non moving creeps

            // printf("CalculateRangedHealThreatInfluence: { ");
            for (int y = threatIMAP.LocalStart.y; y < threatIMAP.LocalEnd.y; ++y)
            {
                for (int x = threatIMAP.LocalStart.x; x < threatIMAP.LocalEnd.x; ++x)
                {
                    int   inLos      = los[localIndex];
                    float threatBase = inLos ? InfluencePrecomputes::ThreatRangedAttack4Falloff[localIndex] : 0;
                    float inf        = threatBase * threat.RangedHealRate * INFLUENCE_THREAT_RANGED_HEAL_FACTOR;
                    threatIMAP.Influence[localIndex] += inf;
                    // if (inf > 0)
                    // {
                    //     // printf("{ MapIndex: %i, InfIndex: %i, InLos: %i, ThreatBase: %f, Inf: %f }, ",
                    //            mapIndex,
                    //            infIndex,
                    //            inLos,
                    //            threatBase,
                    //            inf);
                    // }
                    localIndex++;
                }
                localIndex += localYIncrement;
            }
        }
    };
} // namespace Overseer::Systems::Influence
#endif // OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_THREATINFLUENCE_H_
