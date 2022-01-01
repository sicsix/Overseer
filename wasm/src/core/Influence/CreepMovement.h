//
// Created by Tim on 1/01/2022.
//

#ifndef OVERSEER_WASM_SRC_CORE_INFLUENCE_CREEPMOVEMENT_H_
#define OVERSEER_WASM_SRC_CORE_INFLUENCE_CREEPMOVEMENT_H_
#include "LocalMap.h"
#include "core/Navigation.h"

namespace Overseer::Core::Influence
{
    struct CreepMovement : LocalMap
    {
        Core::Node* Nodes = new Core::Node[INTEREST_SIZE + 1];
    };
}
#endif //OVERSEER_WASM_SRC_CORE_INFLUENCE_CREEPMOVEMENT_H_
