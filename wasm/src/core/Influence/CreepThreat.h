//
// Created by Tim on 1/01/2022.
//

#ifndef OVERSEER_WASM_SRC_CORE_INFLUENCE_CREEPTHREAT_H_
#define OVERSEER_WASM_SRC_CORE_INFLUENCE_CREEPTHREAT_H_
#include "LocalMap.h"
#include "core/LineOfSight.h"
#include "CreepMovement.h"
#include "InfluencePrecomputes.h"

namespace Overseer::Core::Influence
{
    struct CreepThreat : LocalMap
    {
        float* Influence = new float[INFLUENCE_THREAT_SIZE];

        void Calculate(LineOfSight& lineOfSight, Threat& threat, CreepMovement& movement)
        {
            // TODO threat should include hitpoints
            // Attack threat should also depend on ability to move, with no move ability the radius can be 1
            // Is heal a threat? or just an indirect threat? SHould it be included in threat influence?
            if (threat.AttackDamage > 0)
                SetAttackThreat(threat, movement);
            else
                memset(Influence, 0, sizeof(float) * INFLUENCE_THREAT_SIZE);

            if (threat.RangedAttackDamage > 0 || threat.RangedHealRate > 0)
            {
                int los[INFLUENCE_THREAT_SIZE];
                lineOfSight.CalculateLOS(WorldCenter, WorldStart, WorldEnd, los);

                if (threat.RangedAttackDamage > 0)
                    AddRangedAttackThreat(threat, los);

                if (threat.RangedHealRate > 0)
                    AddRangedHealThreat(threat, los);
            }
        }

      private:
        void SetAttackThreat(Components::Threat& threat, CreepMovement& movement)
        {
            int localIndex = LocalStartIndex;
            int movIndex =
                PosToIndex(WorldToLocal(WorldStart, movement.WorldStart), INTEREST_WIDTH) + movement.LocalStartIndex;
            int width           = LocalEnd.x - LocalStart.x;
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
            //     movement.WorldStart.x,
            //     movement.WorldStart.y,
            //     movement.WorldEnd.x,
            //     movement.WorldEnd.y,
            //     movement.WorldCenter.x,
            //     movement.WorldCenter.y,
            //     movement.WorldStartIndex,
            //     movement.LocalStart.x,
            //     movement.LocalStart.y,
            //     movement.LocalEnd.x,
            //     movement.LocalEnd.y,
            //     movement.LocalStartIndex,
            //     movYIncrement,
            //     movIndex);

            // printf("CalculateAttackThreatInfluence: { ");
            for (int y = LocalStart.y; y < LocalEnd.y; ++y)
            {
                for (int x = LocalStart.x; x < LocalEnd.x; ++x)
                {
                    int   moveCost        = movement.Nodes[movIndex].Cost * threat.TicksPerMove;
                    float distBase        = CalculateInverseLinearInfluence(1.0f, moveCost, INFLUENCE_THREAT_RADIUS);
                    float inf             = distBase * threat.AttackDamage * INFLUENCE_THREAT_ATTACK_FACTOR;
                    Influence[localIndex] = inf;
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

        void AddRangedAttackThreat(Threat& threat, int* los)
        {
            int localIndex      = LocalStartIndex;
            int width           = LocalEnd.x - LocalStart.x;
            int localYIncrement = INFLUENCE_THREAT_WIDTH - width;

            // TODO handle non moving creeps

            // printf("CalculateRangedAttackThreatInfluence: { ");
            for (int y = LocalStart.y; y < LocalEnd.y; ++y)
            {
                for (int x = LocalStart.x; x < LocalEnd.x; ++x)
                {
                    int   inLos      = los[localIndex];
                    float threatBase = inLos ? InfluencePrecomputes::ThreatRangedAttack4Falloff[localIndex] : 0;
                    float inf        = threatBase * threat.RangedAttackDamage * INFLUENCE_THREAT_RANGED_ATTACK_FACTOR;
                    Influence[localIndex] += inf;
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

        void AddRangedHealThreat(Threat& threat, int* los)
        {
            int localIndex      = LocalStartIndex;
            int width           = LocalEnd.x - LocalStart.x;
            int localYIncrement = INFLUENCE_THREAT_WIDTH - width;

            // TODO handle non moving creeps

            // printf("CalculateRangedHealThreatInfluence: { ");
            for (int y = LocalStart.y; y < LocalEnd.y; ++y)
            {
                for (int x = LocalStart.x; x < LocalEnd.x; ++x)
                {
                    int   inLos      = los[localIndex];
                    float threatBase = inLos ? InfluencePrecomputes::ThreatRangedAttack4Falloff[localIndex] : 0;
                    float inf        = threatBase * threat.RangedHealRate * INFLUENCE_THREAT_RANGED_HEAL_FACTOR;
                    Influence[localIndex] += inf;
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
} // namespace Overseer::Core::Influence
#endif //OVERSEER_WASM_SRC_CORE_INFLUENCE_CREEPTHREAT_H_
