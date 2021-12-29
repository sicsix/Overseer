//
// Created by Tim on 27/12/2021.
//

#ifndef OVERSEER_WASM_SRC_SYSTEMS_MAP_MAPSYSTEM_H_
#define OVERSEER_WASM_SRC_SYSTEMS_MAP_MAPSYSTEM_H_
#include "systems/SystemBase.h"
#include "components/Components.h"
#include "Signals.h"
#include "core/Navigation.h"
#include "core/Pathfinder.h"
#include "core/Pathcoster.h"
#include "core/LineOfSight.h"

namespace Overseer::Systems
{
    class MapSystem : SystemBase
    {
      public:
        void Initialise(entt::registry& registry) override
        {
            Signals::TerrainMapImportedSink.connect<&MapSystem::TerrainMapImported>(this);
        }

        void Update(entt::registry& registry) override
        {
        }

      private:
        void TerrainMapImported(entt::registry& registry)
        {
            printf("[WASM] Building navigation map...\n");

            auto terrainMap = registry.ctx<Core::TerrainMap>();

            int* navCosts = new int[MAP_SIZE];

            for (int i = 0; i < MAP_SIZE; ++i)
            {
                navCosts[i] = Core::Navigation::TerrainCost[terrainMap.Map[i]];
            }

            auto navMap     = Core::NavMap(navCosts);
            auto pathfinder = Core::Pathfinder(navMap);
            auto pathcoster = Core::Pathcoster(navMap);
            auto lineOfSight = Core::LineOfSight(terrainMap);

            registry.set<Core::NavMap>(navMap);
            registry.set<Core::Pathfinder>(pathfinder);
            registry.set<Core::Pathcoster>(pathcoster);
            registry.set<Core::LineOfSight>(lineOfSight);
        }
    };
} // namespace Overseer::Systems
#endif // OVERSEER_WASM_SRC_SYSTEMS_MAP_MAPSYSTEM_H_
