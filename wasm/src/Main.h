//
// Created by Tim on 23/12/2021.
//

#ifndef OVERSEER_WASM_SRC_MAIN_H_
#define OVERSEER_WASM_SRC_MAIN_H_

#include "Includes.h"
#include "entity/registry.hpp"

namespace Overseer
{
    class Main
    {
      private:
        entt::registry Registry;
        uint*          ImportBuffer;
        uint*          CommandBuffer;

      public:
        int Execute(uint objectCount);

        Main()
        {
        }

        Main(uint* importBuffer, double* commandBuffer);
    };
}; // namespace Overseer

#endif //OVERSEER_WASM_SRC_MAIN_H_
