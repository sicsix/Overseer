//
// Created by Tim on 27/12/2021.
//

#ifndef OVERSEER_WASM_SRC_COMMON_NAVIGATION_H_
#define OVERSEER_WASM_SRC_COMMON_NAVIGATION_H_
#include "Includes.h"

namespace Overseer::Common
{
    class Navigation
    {
      public:
        constexpr static int TerrainCost[] = { 1, INT_MAXVALUE, 5 };
    };
} // namespace Overseer::Common

#endif //OVERSEER_WASM_SRC_COMMON_NAVIGATION_H_
