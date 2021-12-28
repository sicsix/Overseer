//
// Created by Tim on 26/12/2021.
//

#ifndef OVERSEER_WASM_SRC_INCLUDES_H_
#define OVERSEER_WASM_SRC_INCLUDES_H_

#include <cstdio>
#include "linalg.h"

using namespace linalg::aliases;
using namespace linalg;
using uint = uint32_t;
using byte = uint8_t;

constexpr int  MAP_WIDTH  = 100;
constexpr int  MAP_HEIGHT = 100;
constexpr int  MAP_SIZE   = MAP_WIDTH * MAP_HEIGHT;
constexpr int2 MAP_DIMENSIONS(MAP_WIDTH, MAP_HEIGHT);

constexpr int  INFLUENCE_WIDTH         = 13;
constexpr int  INFLUENCE_SIZE          = INFLUENCE_WIDTH * INFLUENCE_WIDTH;
constexpr int  INFLUENCE_MAP_STEP_SIZE = MAP_WIDTH - INFLUENCE_WIDTH;
constexpr int  INFLUENCE_RADIUS        = 6;
constexpr int2 INFLUENCE_CENTER        = int2(INFLUENCE_RADIUS, INFLUENCE_RADIUS);

#endif // OVERSEER_WASM_SRC_INCLUDES_H_
