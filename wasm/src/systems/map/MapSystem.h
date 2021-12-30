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
#include "core/MovementCoster.h"
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
            auto navMap = registry.ctx<Core::NavMap>();
            memcpy(navMap.Map, BaseNavMap.Map, sizeof(int) * MAP_SIZE);
        }

      private:
        Core::NavMap BaseNavMap;

        void TerrainMapImported(entt::registry& registry)
        {
            printf("[WASM] Building navigation map...\n");

            auto terrainMap = registry.ctx<Core::TerrainMap>();

            int* navCosts = new int[MAP_SIZE];

            for (int i = 0; i < MAP_SIZE; ++i)
            {
                navCosts[i] = Core::TerrainCost[terrainMap.Map[i]];
            }

            int* baseNavCosts = new int[MAP_SIZE];
            memcpy(baseNavCosts, navCosts, sizeof(int) * MAP_SIZE);
            BaseNavMap = Core::NavMap(baseNavCosts);

            auto navMap         = Core::NavMap(navCosts);
            auto pathfinder     = Core::Pathfinder(navMap);
            auto movementCoster = Core::MovementCoster(navMap);
            auto lineOfSight    = Core::LineOfSight(terrainMap);

            registry.set<Core::NavMap>(navMap);
            registry.set<Core::Pathfinder>(pathfinder);
            registry.set<Core::MovementCoster>(movementCoster);
            registry.set<Core::LineOfSight>(lineOfSight);
        }
    };
} // namespace Overseer::Systems
#endif // OVERSEER_WASM_SRC_SYSTEMS_MAP_MAPSYSTEM_H_
