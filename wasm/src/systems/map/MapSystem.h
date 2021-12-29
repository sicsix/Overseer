//
// Created by Tim on 27/12/2021.
//

#ifndef OVERSEER_WASM_SRC_SYSTEMS_MAP_MAPSYSTEM_H_
#define OVERSEER_WASM_SRC_SYSTEMS_MAP_MAPSYSTEM_H_
#include "systems/SystemBase.h"
#include "components/Components.h"
#include "Signals.h"
#include "common/Navigation.h"
#include "common/Pathfinder.h"
#include "common/Pathcoster.h"
#include "common/LineOfSight.h"

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

            auto terrainMap = registry.ctx<Common::TerrainMap>();

            int* navCosts = new int[MAP_SIZE];

            for (int i = 0; i < MAP_SIZE; ++i)
            {
                navCosts[i] = Common::Navigation::TerrainCost[terrainMap.Map[i]];
            }

            auto navMap     = Common::NavMap(navCosts);
            auto pathfinder = Common::Pathfinder(navMap);
            auto pathcoster = Common::Pathcoster(navMap);
            auto lineOfSight = Common::LineOfSight(terrainMap);

            registry.set<Common::NavMap>(navMap);
            registry.set<Common::Pathfinder>(pathfinder);
            registry.set<Common::Pathcoster>(pathcoster);
            registry.set<Common::LineOfSight>(lineOfSight);
        }
    };
} // namespace Overseer::Systems
#endif // OVERSEER_WASM_SRC_SYSTEMS_MAP_MAPSYSTEM_H_
