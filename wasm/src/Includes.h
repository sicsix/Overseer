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

constexpr int INT_MAXVALUE = 2147483647;

constexpr int  MAP_WIDTH  = 100;
constexpr int  MAP_HEIGHT = 100;
constexpr int  MAP_SIZE   = MAP_WIDTH * MAP_HEIGHT;
constexpr int2 MAP_DIMENSIONS(MAP_WIDTH, MAP_HEIGHT);

constexpr int  INFLUENCE_WIDTH         = 13;
constexpr int  INFLUENCE_SIZE          = INFLUENCE_WIDTH * INFLUENCE_WIDTH;
constexpr int  INFLUENCE_MAP_STEP_SIZE = MAP_WIDTH - INFLUENCE_WIDTH;
constexpr int  INFLUENCE_RADIUS        = 6;
constexpr int2 INFLUENCE_CENTER        = int2(INFLUENCE_RADIUS, INFLUENCE_RADIUS);

constexpr float INFLUENCE_ATTACK_THREAT_MULT = 0.005f;
constexpr float INFLUENCE_RANGED_ATTACK_THREAT_MULT = 0.005f;
constexpr float INFLUENCE_RANGED_HEAL_THREAT_MULT = 0.0025f;

enum struct Direction
{
    TOP          = 1,
    TOP_RIGHT    = 2,
    RIGHT        = 3,
    BOTTOM_RIGHT = 4,
    BOTTOM       = 5,
    BOTTOM_LEFT  = 6,
    LEFT         = 7,
    TOP_LEFT     = 8
};

enum struct Terrain
{
    PLAIN = 0,
    WALL  = 1,
    SWAMP = 2,
    NONE  = 3
};

#endif // OVERSEER_WASM_SRC_INCLUDES_H_
