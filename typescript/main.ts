import { arenaInfo } from "game";
import { getCpuTime } from "game/utils";
import { ModuleManager } from "./moduleManager";
import { CreepExporter } from "./exporters/creepExporter";
import { TerrainMapExporter } from "./exporters/terrainMapExporter";

const moduleManager: ModuleManager = new ModuleManager();
const terrainMapExporter: TerrainMapExporter = new TerrainMapExporter(moduleManager.ExportBuffer);
const creepExporter: CreepExporter = new CreepExporter(moduleManager.ExportBuffer);


const NS_TO_MS = 1 / 1000000;

let initialised = false;

export function loop(): void
{
    let timeLimit = initialised ? arenaInfo.cpuTimeLimit : arenaInfo.cpuTimeLimitFirstTick;
    if (!initialised) Initialise(); else moduleManager.Begin();

    creepExporter.Run();

    moduleManager.Execute();


    console.log("__________________________________________");
    console.log(`[CPU MONITOR]  TIME: ${(getCpuTime() * NS_TO_MS).toFixed(3)}ms  USAGE: ${(getCpuTime() / timeLimit * 100).toFixed(2)}%`);
    console.log("------------------------------------------");
}

function Initialise(): void
{
    initialised = true;
    moduleManager.Initialise();

    terrainMapExporter.Run();
}


