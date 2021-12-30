//
// Created by Tim on 26/12/2021.
//

#ifndef OVERSEER_WASM_SRC_COMPONENTS_COMPONENTS_H_
#define OVERSEER_WASM_SRC_COMPONENTS_COMPONENTS_H_
#include "Includes.h"
#include "core/Navigation.h"
#include "core/Influence.h"

namespace Overseer::Components
{
    struct My
    {
    };

    struct Pos
    {
        int2 Val;

        Pos(const int2& val): Val(val)
        {
        }
    };

    struct Health
    {
        int Hits;
        int HitsMax;

        Health(const int& hits, const int& hitsMax): Hits(hits), HitsMax(hitsMax)
        {
        }
    };

    enum struct BodyPartType : byte
    {
        NONE          = 0,
        MOVE          = 1,
        WORK          = 2,
        CARRY         = 3,
        ATTACK        = 4,
        RANGED_ATTACK = 5,
        TOUGH         = 6,
        HEAL          = 7
    };

    struct BodyPart
    {
        byte         Hits;
        BodyPartType Type;
    };

    struct Body
    {
        int      Count;
        BodyPart Parts[50];

        Body(int count, BodyPart* bodyParts): Count(count)
        {
            for (int i = 0; i < count; ++i)
            {
                Parts[i] = bodyParts[i];
            }
        }
    };

    struct Attributes
    {
        int TicksPerMove = 0;
        int Move         = 0;
        int Work         = 0;
        int Carry        = 0;
        int Attack       = 0;
        int RangedAttack = 0;
        int Tough        = 0;
        int Heal         = 0;

        Attributes(Body& body)
        {
            int totalNonMove = 0;

            for (int i = 0; i < body.Count; ++i)
            {
                const BodyPart part = body.Parts[i];

                switch (part.Type)
                {
                    case BodyPartType::NONE:
                        break;
                    case BodyPartType::MOVE:
                        if (part.Hits > 0)
                            Move++;
                        break;
                    case BodyPartType::WORK:
                        if (part.Hits > 0)
                            Work++;
                        totalNonMove++;
                        break;
                    case BodyPartType::CARRY:
                        if (part.Hits > 0)
                            Carry++;
                        break;
                    case BodyPartType::ATTACK:
                        if (part.Hits > 0)
                            Attack++;
                        totalNonMove++;
                        break;
                    case BodyPartType::RANGED_ATTACK:
                        if (part.Hits > 0)
                            RangedAttack++;
                        totalNonMove++;
                        break;
                    case BodyPartType::TOUGH:
                        if (part.Hits > 0)
                            Tough++;
                        totalNonMove++;
                        break;
                    case BodyPartType::HEAL:
                        if (part.Hits > 0)
                            Heal++;
                        totalNonMove++;
                        break;
                }
            }

            TicksPerMove = Move == 0 ? -1 : min((int)ceil((float)totalNonMove / (float)Move), 1);
        }
    };

    struct Threat
    {
        int TicksPerMove;
        int AttackDamage;
        int RangedAttackDamage;
        int RangedMassAttackDamage1;
        int RangedMassAttackDamage2;
        int RangedMassAttackDamage3;
        int HealRate;
        int RangedHealRate;

        Threat(Attributes& attributes)
        {
            TicksPerMove            = attributes.TicksPerMove;
            AttackDamage            = attributes.Attack * 30;
            RangedAttackDamage      = attributes.RangedAttack * 10;
            RangedMassAttackDamage1 = attributes.RangedAttack * 10;
            RangedMassAttackDamage2 = attributes.RangedAttack * 4;
            RangedMassAttackDamage3 = attributes.RangedAttack;
            HealRate                = attributes.Heal * 12;
            RangedHealRate          = attributes.Heal * 4;
        }
    };

    struct Path
    {
        static constexpr int Size = 128;

        int2 Val[Size];
        //		std::array<int2, 128> Val;

        int Count = 0;

        void Add(int2& pos)
        {
            Val[Count++] = pos;
        }

        void Clear()
        {
            Count = 0;
        }
    };

    struct CreepProxIMAP : Core::LocalMap
    {
        float* Influence = new float[INFLUENCE_PROX_SIZE];
    };

    struct CreepThreatIMAP : Core::LocalMap
    {
        float* Influence = new float[INFLUENCE_THREAT_SIZE];
    };

    struct CreepMovementMap : Core::LocalMap
    {
        Core::Node* Nodes = new Core::Node[INTEREST_SIZE + 1];
    };

} // namespace Overseer::Components

using namespace Overseer::Components;
#endif // OVERSEER_WASM_SRC_COMPONENTS_COMPONENTS_H_
