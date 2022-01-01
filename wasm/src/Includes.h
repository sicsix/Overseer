//
// Created by Tim on 26/12/2021.
//

#ifndef OVERSEER_WASM_SRC_INCLUDES_H_
#define OVERSEER_WASM_SRC_INCLUDES_H_

#define DEBUG_ENABLED

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

constexpr int  INFLUENCE_PROX_WIDTH         = 9;
constexpr int  INFLUENCE_PROX_SIZE          = INFLUENCE_PROX_WIDTH * INFLUENCE_PROX_WIDTH;
constexpr int  INFLUENCE_PROX_MAP_STEP_SIZE = MAP_WIDTH - INFLUENCE_PROX_WIDTH;
constexpr int  INFLUENCE_PROX_RADIUS        = 4;
constexpr int2 INFLUENCE_PROX_CENTER        = int2(INFLUENCE_PROX_RADIUS, INFLUENCE_PROX_RADIUS);

constexpr int  INFLUENCE_THREAT_WIDTH         = 13;
constexpr int  INFLUENCE_THREAT_SIZE          = INFLUENCE_THREAT_WIDTH * INFLUENCE_THREAT_WIDTH;
constexpr int  INFLUENCE_THREAT_MAP_STEP_SIZE = MAP_WIDTH - INFLUENCE_THREAT_WIDTH;
constexpr int  INFLUENCE_THREAT_RADIUS        = 6;
constexpr int2 INFLUENCE_THREAT_CENTER        = int2(INFLUENCE_THREAT_RADIUS, INFLUENCE_THREAT_RADIUS);

constexpr float INFLUENCE_THREAT_ATTACK_FACTOR        = 0.005f;
constexpr float INFLUENCE_THREAT_RANGED_ATTACK_FACTOR = 0.005f;
constexpr float INFLUENCE_THREAT_RANGED_HEAL_FACTOR   = 0.0025f;

constexpr int  INTEREST_WIDTH     = 17;
constexpr int  INTEREST_SIZE      = INTEREST_WIDTH * INTEREST_WIDTH;
constexpr int  INTEREST_STEP_SIZE = MAP_WIDTH - INTEREST_WIDTH;
constexpr int  INTEREST_RADIUS    = 8;
constexpr int2 INTEREST_CENTER    = int2(INTEREST_RADIUS, INTEREST_RADIUS);

constexpr int MAX_SQUAD_SIZE = 20;

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
