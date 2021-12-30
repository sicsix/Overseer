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
            int mapIndex = threatIMAP.MapStartIndex;
            int infIndex = threatIMAP.InfStartIndex;
            int movIndex = ;

            // printf("CalculateAttackThreatInfluence: { ");
            for (int y = threatIMAP.InfStart.y; y < threatIMAP.InfEnd.y; ++y)
            {
                for (int x = threatIMAP.InfStart.x; x < threatIMAP.InfEnd.x; ++x)
                {
                    int   moveCost = min(creepMovementMap.Costs[movIndex] * threat.TicksPerMove, INFLUENCE_THREAT_RADIUS);
                    float distBase = CalculateInverseLinearInfluence(1.0f, moveCost, INFLUENCE_THREAT_RADIUS);
                    float inf      = distBase * threat.AttackDamage * INFLUENCE_THREAT_ATTACK_FACTOR;
                    threatIMAP.Influence[infIndex] = inf;
                    // printf("{ MapIndex: %i, InfIndex: %i, Cost: %i, DistBase: %f, Inf: %f }, ",
                    //        mapIndex,
                    //        infIndex,
                    //        cost,
                    //        distBase,
                    //        inf);
                    mapIndex++;
                    infIndex++;
                    movIndex++;
                }
                mapIndex += threatIMAP.MapYIncrement;
                infIndex += threatIMAP.InfYIncrement;
                movIndex += movYIncrement;
            }
            // printf("} \n");
        }

        static void AddRangedAttackThreat(CreepThreatIMAP& threatIMAP, Threat& threat, int* los)
        {
            int mapIndex = threatIMAP.MapStartIndex;
            int infIndex = threatIMAP.InfStartIndex;

            // TODO handle non moving creeps

            // printf("CalculateRangedAttackThreatInfluence: { ");
            for (int y = threatIMAP.InfStart.y; y < threatIMAP.InfEnd.y; ++y)
            {
                for (int x = threatIMAP.InfStart.x; x < threatIMAP.InfEnd.x; ++x)
                {
                    int   inLos      = los[infIndex];
                    float threatBase = inLos ? InfluencePrecomputes::ThreatRangedAttack4Falloff[infIndex] : 0;
                    float inf        = threatBase * threat.RangedAttackDamage * INFLUENCE_THREAT_RANGED_ATTACK_FACTOR;
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

        static void AddRangedHealThreat(CreepThreatIMAP& threatIMAP, Threat& threat, int* los)
        {
            int mapIndex = threatIMAP.MapStartIndex;
            int infIndex = threatIMAP.InfStartIndex;

            // TODO handle non moving creeps

            // printf("CalculateRangedHealThreatInfluence: { ");
            for (int y = threatIMAP.InfStart.y; y < threatIMAP.InfEnd.y; ++y)
            {
                for (int x = threatIMAP.InfStart.x; x < threatIMAP.InfEnd.x; ++x)
                {
                    int   inLos      = los[infIndex];
                    float threatBase = inLos ? InfluencePrecomputes::ThreatRangedAttack4Falloff[infIndex] : 0;
                    float inf        = threatBase * threat.RangedHealRate * INFLUENCE_THREAT_RANGED_HEAL_FACTOR;
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
        }
    };
} // namespace Overseer::Systems::Influence
#endif // OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_THREATINFLUENCE_H_
