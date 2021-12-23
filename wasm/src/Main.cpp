#include "Main.h"
#include "Signals.h"
#include "commands/CommandHandler.h"
#include "imports/ImportHandler.h"
#include "common/Structures.h"
#include "systems/map/MapSystem.h"
#include "systems/movement/MovementSystem.h"
#include "systems/influence/InfluenceSystem.h"
#include "common/Pathfinder.h"

namespace Overseer
{
    Systems::MapSystem       mapSystem;
    Systems::MovementSystem  movementSystem;
    Systems::InfluenceSystem influenceSystem;

    Main::Main(uint* importBuffer, double* commandBuffer): ImportBuffer(importBuffer)
    {
        Commands::CommandHandler::SetBuffer(commandBuffer);

        Signals();

        mapSystem.Initialise(Registry);
        influenceSystem.Initialise(Registry);
        movementSystem.Initialise(Registry);
    }

    int Main::Execute(uint objectCount)
    {
        Imports::ImportHandler::Process(Registry, ImportBuffer, objectCount);
        Commands::CommandHandler::Begin();

        mapSystem.Update(Registry);
        influenceSystem.Update(Registry);
        movementSystem.Update(Registry);

        auto start = int2(10, 10);
        auto goal  = int2(90, 90);

        auto pathFinder = Registry.ctx<Common::Pathfinder>();
        auto path       = Components::Path();

        for (int i = 0; i < 14; ++i)
        {
            bool foundPath = pathFinder.FindPath(start, goal, path);

            if (!foundPath)
                printf("[WASM] Failed to find path\n");
        }

        // printf("Path:\n");
        // for (int i = 0; i < path.Count; ++i)
        // {
        //     int2 pos = path.Val[i];
        //     printf(" {%i, %i} ", pos.x, pos.y);
        // }
        // printf("\n");

        //		auto group = Registry.group<Pos, Health>();
        //
        //		for (auto entity : group)
        //		{
        //			auto [pos, health] = group.Pop(entity);
        //
        //			printf("Entity: %u - Pos: { %u, %u } - Health: { %u, %u}\n", entity, pos.Val.x, pos.Val.y,
        // health.Hits, health.HitsMax);
        //		}

        //		Pos* pos = group.raw<Pos>();
        //		Health* health = group.raw<velocity>();

        //		auto terrainMap = Registry.ctx<Structures::TerrainMap>().Map;
        //
        //		for (int y = 0; y < 25; ++y)
        //		{
        //			for (int x = 0; x < 100; ++x)
        //			{
        //				auto index = y * 100 + x;
        //				auto terrain = terrainMap[index];
        //				if (terrain == 1)
        //					Commands::CommandHandler::Add(Commands::GameVisualRect({ x, y }, { 1, 1 },
        // Commands::Colour::WHITE, 1));
        //			}
        //		}

        return Commands::CommandHandler::End();
    }
} // namespace Overseer

Overseer::Main MainInstance;

extern "C"
{
    [[maybe_unused]] bool Initialise(uint* importBuffer, double* commandBuffer)
    {
        printf("[WASM] Initialising Overseer...\n");
        MainInstance = Overseer::Main(importBuffer, commandBuffer);
        printf("[WASM] Initialisation complete\n");
        return true;
    }

    [[maybe_unused]] int Loop(int messageCount)
    {
        return MainInstance.Execute(messageCount);
    }
}
