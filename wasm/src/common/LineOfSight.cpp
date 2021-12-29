//
// Created by Tim on 29/12/2021.
//

#include "LineOfSight.h"
#include "Math.h"

namespace Overseer::Common
{
    LineOfSight::LineOfSight(struct TerrainMap& terrainMap)
    {
        TerrainMap = terrainMap;
    }

    struct LineOfSight::Row
    {
        int   Depth;
        int2  StartSlopeFraction;
        int2  EndSlopeFraction;
        float StartSlope;
        float EndSlope;

        Row()
        {
        }

        Row(int depth, int2 startSlopeFraction, int2 endSlopeFraction)
        {
            Depth              = depth;
            StartSlopeFraction = startSlopeFraction;
            EndSlopeFraction   = endSlopeFraction;
            StartSlope         = (float)startSlopeFraction.x / (float)startSlopeFraction.y;
            EndSlope           = (float)endSlopeFraction.x / (float)endSlopeFraction.y;
        }
    };

    struct LineOfSight::Tile
    {
        int RowDepth;
        int Column;

        Tile(int rowDepth, int column): RowDepth(rowDepth), Column(column)
        {
        }
    };

    struct LineOfSight::RowEnumerator
    {
      private:
        int Depth;
        int Column;
        int MinColumn;
        int MaxColumn;

      public:
        RowEnumerator(const struct Row& row)
        {
            Depth     = row.Depth;
            MinColumn = (int)floor((float)row.Depth * row.StartSlope + 0.5f);
            MaxColumn = (int)ceil((float)row.Depth * row.EndSlope - 0.5f);
            Column    = MinColumn - 1;
        }

        bool MoveNext()
        {
            if (Column < MaxColumn)
                Column++;
            else
                return false;
            return true;
        }

        void Reset()
        {
            Column = MinColumn - 1;
        }

        Tile Current()
        {
            return Tile(Depth, Column);
        }
    };

    struct LineOfSight::Quadrant
    {
      private:
        Direction Direction;
        int2      Origin;

      public:
        Quadrant(enum Direction direction, const int2& origin): Direction(direction), Origin(origin)
        {
        }

        int2 Transform(Tile tile)
        {
            switch (Direction)
            {
                case Direction::TOP:
                    return int2(Origin.x + tile.Column, Origin.y - tile.RowDepth);
                case Direction::RIGHT:
                    return int2(Origin.x + tile.RowDepth, Origin.y + tile.Column);
                case Direction::BOTTOM:
                    return int2(Origin.x + tile.Column, Origin.y + tile.RowDepth);
                case Direction::LEFT:
                    return int2(Origin.x - tile.RowDepth, Origin.y + tile.Column);
                default:
                    throw;
            }
        }
    };
    bool LineOfSight::IsWall(Terrain terrain)
    {
        if (terrain == Terrain::NONE)
            return false;
        return terrain == Terrain::WALL;
    }
    bool LineOfSight::IsFloor(Terrain terrain)
    {
        if (terrain == Terrain::NONE)
            return false;
        return terrain != Terrain::WALL;
    }
    bool LineOfSight::IsSymmetric(LineOfSight::Row& row, LineOfSight::Tile& tile)
    {
        return (tile.Column >= row.Depth * row.StartSlope) && (tile.Column <= row.Depth * row.EndSlope);
    }
    int2 LineOfSight::Slope(LineOfSight::Tile& tile)
    {
        return int2(2 * tile.Column - 1, 2 * tile.RowDepth);
    }

    void LineOfSight::CalculateLOS(CreepThreatIMAP& threatIMAP, int* vision)
    {
        memset(vision, 0, sizeof(int) * INFLUENCE_SIZE);

        int infStartIndex     = PosToIndex(INFLUENCE_CENTER, INFLUENCE_WIDTH);
        vision[infStartIndex] = 1;

        // printf("Calculating LOS...\n");

        Row rows[INFLUENCE_WIDTH * 8]; // More than needed?
        int currRowIndex = 0;
        for (int i = 1; i < 9; i += 2)
        {
            auto quadrant        = Quadrant((Direction)i, threatIMAP.WorldCenter);
            rows[currRowIndex++] = Row(1, int2(-1, 1), int2(1, 1));
            // printf("Direction: %i\n", i);
            while (currRowIndex > 0)
            {
                auto    row                = rows[--currRowIndex];
                Terrain prevTerrain        = Terrain::NONE;
                auto    startSlopeFraction = row.StartSlopeFraction;
                auto    enumerator         = RowEnumerator(row);

                // printf("   CurrentRowIndex: %i    PrevTerrain: %i    StartSlopeFraction: { %i, %i }\n",
                //        currRowIndex,
                //        prevTerrain,
                //        startSlopeFraction.x,
                //        startSlopeFraction.y);

                while (enumerator.MoveNext())
                {
                    auto tile     = enumerator.Current();
                    auto worldPos = quadrant.Transform(tile);
                    if (!IsPosInbounds(worldPos, threatIMAP.WorldStart, threatIMAP.WorldEnd))
                        continue;
                    auto worldIndex = PosToIndex(worldPos, MAP_WIDTH);
                    auto terrain    = (Terrain)TerrainMap.Map[worldIndex];

                    // printf(
                    //     "        Tile: { Column: %i, Row Depth %i}    WorldPos: { %i, %i }    WorldIndex: %i    Terrain: %i\n",
                    //     tile.Column,
                    //     tile.RowDepth,
                    //     worldPos.x,
                    //     worldPos.y,
                    //     worldIndex,
                    //     terrain);

                    if (IsSymmetric(row, tile) && IsFloor(terrain))
                    {
                        auto infPos      = worldPos - threatIMAP.WorldStart;
                        auto infIndex    = PosToIndex(infPos, INFLUENCE_WIDTH);
                        vision[infIndex] = 1;
                    }

                    if (IsWall(prevTerrain) && IsFloor(terrain))
                    {
                        startSlopeFraction = Slope(tile);
                        // printf("            Changed StartSlopeFraction: { %i, %i }\n",
                        //        startSlopeFraction.x,
                        //        startSlopeFraction.y);
                    }
                    if (IsFloor(prevTerrain) && IsWall(terrain))
                    {
                        if (row.Depth < INFLUENCE_RADIUS)
                        {
                            int  depth            = row.Depth + 1;
                            int2 endSlopeFraction = Slope(tile);
                            // printf(
                            //     "            Adding row due to block - Depth: %i    StartSlopeFraction: { %i, %i }    EndSlopeFraction: { %i, %i }\n",
                            //     depth,
                            //     startSlopeFraction.x,
                            //     startSlopeFraction.y,
                            //     endSlopeFraction.x,
                            //     endSlopeFraction.y);
                            rows[currRowIndex++] = Row(depth, startSlopeFraction, endSlopeFraction);
                        }
                    }

                    prevTerrain = terrain;
                }
                if (IsFloor(prevTerrain) && row.Depth < INFLUENCE_RADIUS)
                {
                    // printf(
                    //     "    Adding next row - Depth: %i    StartSlopeFraction: { %i, %i }    EndSlopeFraction: { %i, %i }\n",
                    //     row.Depth + 1,
                    //     startSlopeFraction.x,
                    //     startSlopeFraction.y,
                    //     row.EndSlopeFraction.x,
                    //     row.EndSlopeFraction.y);
                    rows[currRowIndex++] = Row(row.Depth + 1, startSlopeFraction, row.EndSlopeFraction);
                }
            }
        }
    }
} // namespace Overseer::Common