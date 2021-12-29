//
// Created by Tim on 29/12/2021.
//

#ifndef OVERSEER_WASM_SRC_COMMON_LINEOFSIGHT_H_
#define OVERSEER_WASM_SRC_COMMON_LINEOFSIGHT_H_
#include "Includes.h"
#include "Structures.h"
#include "components/Components.h"

namespace Overseer::Common
{
    class LineOfSight
    {
      private:
        TerrainMap TerrainMap;

      public:
        LineOfSight(struct TerrainMap& terrainMap);

        void CalculateLOS(CreepThreatIMAP& threatIMAP, int* vision);

      private:
        struct Row;
        struct Tile;

        struct RowEnumerator;

        struct Quadrant;

        static bool IsWall(Terrain terrain);

        static bool IsFloor(Terrain terrain);

        static bool IsSymmetric(Row& row, Tile& tile);

        static int2 Slope(Tile& tile);
    };
} // namespace Overseer::Common

#endif // OVERSEER_WASM_SRC_COMMON_LINEOFSIGHT_H_
