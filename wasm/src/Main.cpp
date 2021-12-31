#include "Main.h"
#include "Signals.h"
#include "commands/CommandHandler.h"
#include "imports/ImportHandler.h"
#include "core/Structures.h"
#include "systems/map/MapSystem.h"
#include "systems/movement/MovementSystem.h"
#include "systems/influence/InfluenceSystem.h"
#include "core/Pathfinder.h"
#include "systems/ai/GruntSystem.h"
#include "systems/ai/SquadSystem.h"

namespace Overseer
{
    Systems::MapSystem                  mapSystem;
    Systems::Influence::InfluenceSystem influenceSystem;
    Systems::AI::SquadSystem            squadSystem;
    Systems::AI::GruntSystem            gruntSystem;
    Systems::MovementSystem             movementSystem;

    Main::Main(uint* importBuffer, double* commandBuffer): ImportBuffer(importBuffer)
    {
        Commands::CommandHandler::SetBuffer(commandBuffer);

        rand();
        Signals();

        mapSystem.Initialise(Registry);
        influenceSystem.Initialise(Registry);
        squadSystem.Initialise(Registry);
        gruntSystem.Initialise(Registry);
        movementSystem.Initialise(Registry);
    }

    int Main::Execute(uint objectCount)
    {
        Imports::ImportHandler::Process(Registry, ImportBuffer, objectCount);
        Commands::CommandHandler::Begin();

        printf("[WASM] Running MapSystem...\n");
        mapSystem.Update(Registry);

        printf("[WASM] Running InfluenceSystem...\n");
        influenceSystem.Update(Registry);

        printf("[WASM] Running SquadSystem...\n");
        squadSystem.Update(Registry);

        printf("[WASM] Running GruntSystem...\n");
        gruntSystem.Update(Registry);

        printf("[WASM] Running MovementSystem...\n");
        movementSystem.Update(Registry);

        printf("[WASM] Finalising command buffer...\n");
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
