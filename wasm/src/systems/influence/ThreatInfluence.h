//
// Created by Tim on 30/12/2021.
//

#ifndef OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_THREATINFLUENCE_H_
#define OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_THREATINFLUENCE_H_
#include "components/Components.h"
#include "core/Pathcoster.h"
#include "core/LineOfSight.h"

namespace Overseer::Systems::Influence
{
    static void CalculateThreatInfluence(CreepThreatIMAP&     threatIMAP,
                                         Core::LineOfSight& lineOfSight,
                                         Threat               threat,
                                         int*                 costs)
    {
        // TODO threat should include hitpoints

        // Three types of threat exist -> Heal, Attack, RangedAttack
        // Each should have a different splat and sum them together
        // Do we need to do pathfinding to determine attack threat?
        // Do we need to do visibility checks to determine ranged attack threat?
        // Is heal threat just a constant that ignores vis, but keeps terrain?
        // Maybe set some constants to determine what each part is worth threat wise
        // Attack threat should also depend on ability to move, with no move ability the radius can be 1

        // Is heal a threat? or just an indirect threat? SHould it be included in threat influence?

        if (threat.AttackDamage > 0)
        {
            int mapIndex = threatIMAP.MapStartIndex;
            int infIndex = threatIMAP.InfStartIndex;

            // printf("CalculateAttackThreatInfluence: { ");
            for (int y = threatIMAP.InfStart.y; y < threatIMAP.InfEnd.y; ++y)
            {
                for (int x = threatIMAP.InfStart.x; x < threatIMAP.InfEnd.x; ++x)
                {
                    int   tileCost                 = min(costs[infIndex], 7);
                    float distBase                 = Core::CalculateLinearLookup(tileCost);
                    float inf                      = distBase * threat.AttackDamage * INFLUENCE_ATTACK_THREAT_MULT;
                    threatIMAP.Influence[infIndex] = inf;
                    // printf("{ MapIndex: %i, InfIndex: %i, Cost: %i, DistBase: %f, Inf: %f }, ",
                    //        mapIndex,
                    //        infIndex,
                    //        cost,
                    //        distBase,
                    //        inf);
                    mapIndex++;
                    infIndex++;
                }
                mapIndex += threatIMAP.MapYIncrement;
                infIndex += threatIMAP.InfYIncrement;
            }
            // printf("} \n");
        }
        else
            memset(threatIMAP.Influence, 0, sizeof(float) * INFLUENCE_SIZE);

        if (threat.RangedAttackDamage > 0 || threat.RangedHealRate > 0)
            lineOfSight.CalculateLOS(threatIMAP, costs);

        // Calcalute RANGED threat
        if (threat.RangedAttackDamage > 0)
        {
            int mapIndex = threatIMAP.MapStartIndex;
            int infIndex = threatIMAP.InfStartIndex;

            // TODO handle non moving creeps

            // printf("CalculateRangedAttackThreatInfluence: { ");
            for (int y = threatIMAP.InfStart.y; y < threatIMAP.InfEnd.y; ++y)
            {
                for (int x = threatIMAP.InfStart.x; x < threatIMAP.InfEnd.x; ++x)
                {
                    int   inLos      = costs[infIndex];
                    float threatBase = inLos ? Core::InfluencePrecomputes::RangedAttack4Falloff[infIndex] : 0;
                    float inf        = threatBase * threat.RangedAttackDamage * INFLUENCE_RANGED_ATTACK_THREAT_MULT;
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

        if (threat.RangedHealRate > 0)
        {
            int mapIndex = threatIMAP.MapStartIndex;
            int infIndex = threatIMAP.InfStartIndex;

            // TODO handle non moving creeps

            // printf("CalculateRangedHealThreatInfluence: { ");
            for (int y = threatIMAP.InfStart.y; y < threatIMAP.InfEnd.y; ++y)
            {
                for (int x = threatIMAP.InfStart.x; x < threatIMAP.InfEnd.x; ++x)
                {
                    int   inLos      = costs[infIndex];
                    float threatBase = inLos ? Core::InfluencePrecomputes::RangedAttack4Falloff[infIndex] : 0;
                    float inf        = threatBase * threat.RangedHealRate * INFLUENCE_RANGED_HEAL_THREAT_MULT;
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
    }
} // namespace Overseer::Systems::Influence
#endif // OVERSEER_WASM_SRC_SYSTEMS_INFLUENCE_THREATINFLUENCE_H_
