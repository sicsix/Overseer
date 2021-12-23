//
// Created by Tim on 26/12/2021.
//

#ifndef OVERSEER_WASM_SRC_SYSTEMS_SYSTEMBASE_H_
#define OVERSEER_WASM_SRC_SYSTEMS_SYSTEMBASE_H_
#include "entity/registry.hpp"

namespace Overseer::Systems
{
    class SystemBase
    {
      public:
        virtual void Initialise(entt::registry& registry) = 0;
        virtual void Update(entt::registry& registry)     = 0;
    };
} // namespace Overseer::Systems

#endif //OVERSEER_WASM_SRC_SYSTEMS_SYSTEMBASE_H_
