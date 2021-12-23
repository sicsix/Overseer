#include "ImportHandler.h"
#include "common/Structures.h"
#include "components/Components.h"
#include "Signals.h"

namespace Overseer::Imports
{
    void ImportHandler::Process(entt::registry& registry, uint* buffer, int objectCount)
    {
        printf("[WASM] Processing %i imports...\n", objectCount);
        int currIndex = 0;

        if (buffer[currIndex++] != ExportFlags::BEGIN)
            throw printf("[WASM] Expected BEGIN\n");

        for (int currObject = 0; currObject < objectCount; ++currObject)
        {
            auto exportFlags = (ExportFlags)buffer[currIndex++];
            if ((exportFlags & ExportFlags::NEXT) == 0)
                throw printf("[WASM] Expected NEXT\n");

            ImportHandler::ProcessExport(registry, buffer, currIndex, currObject, exportFlags);
        }

        if (buffer[currIndex] != ExportFlags::END)
            throw printf("[WASM] Expected END\n");
    }
    void ImportHandler::ProcessExport(entt::registry& registry,
                                      uint*           buffer,
                                      int&            currIndex,
                                      int&            curObject,
                                      ExportFlags     exportFlags)
    {
        exportFlags = (ExportFlags)(exportFlags - ExportFlags::NEXT);

        switch (exportFlags)
        {
            case ExportFlags::MAP:
                ImportTerrainMap(registry, buffer, currIndex, curObject);
                break;
            case ExportFlags::CREEP:
                ImportCreeps(registry, buffer, currIndex, curObject);
                break;
            case ExportFlags::DESTROYED:
            case ExportFlags::BEGIN:
            case ExportFlags::END:
            case ExportFlags::NEXT:
            case ExportFlags::NONE:
            default:
                throw printf("[WASM] Expected valid export type\n");
        }
    }
    void ImportHandler::ImportTerrainMap(entt::registry& registry, uint* buffer, int& currIndex, int& curObject)
    {
        printf("[WASM] Importing terrain map...\n");
        curObject++;

        auto size = buffer[currIndex++];

        auto incTerrain = &buffer[currIndex];
        auto newTerrain = new int[size];
        memcpy(newTerrain, incTerrain, sizeof(int) * size);
        currIndex += size;

        auto terrainMap = Common::TerrainMap(newTerrain);

        registry.set<Common::TerrainMap>(terrainMap);

        Signals::TerrainMapImported.publish(registry);
    }

    void ImportHandler::ImportCreeps(entt::registry& registry, uint* buffer, int& currIndex, int& curObject)
    {
        auto count = buffer[currIndex++];
        curObject += count;

        printf("[WASM] Importing %i creeps...\n", count);

        for (int i = 0; i < count; ++i)
        {
            auto entity  = (entt::entity)buffer[currIndex++];
            auto my      = buffer[currIndex++] == 1;
            auto x       = buffer[currIndex++];
            auto y       = buffer[currIndex++];
            auto hits    = buffer[currIndex++];
            auto hitsMax = buffer[currIndex++];

            if (!registry.valid(entity))
            {
                auto createdEntity = registry.create(entity);
                if (createdEntity != entity)
                    throw printf("[WASM] Created Entity does not match imported Entity\n");

                if (my)
                {
                    registry.emplace<My>(entity);
                    registry.emplace<Path>(entity);
                }

                registry.emplace<CreepThreatIMAP>(entity);
                registry.emplace<CreepProxIMAP>(entity);
            }

            registry.emplace_or_replace<Pos>(entity, Pos({ x, y }));
            registry.emplace_or_replace<Health>(entity, Health(hits, hitsMax));

            auto bodyLength = buffer[currIndex++];

            // TODO add code here to decode body length

            currIndex += (bodyLength >> 1) + bodyLength % 2;
        }
    }
} // namespace Overseer::Imports
