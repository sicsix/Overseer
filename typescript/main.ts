import { arenaInfo } from "game";
import { getCpuTime, getObjectsByPrototype } from "game/utils";
import { ModuleManager } from "./moduleManager";
import { CreepExporter } from "./exporters/creepExporter";
import { TerrainMapExporter } from "./exporters/terrainMapExporter";
import { Creep, StructureTower } from "game/prototypes";

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

    // let creeps = getObjectsByPrototype(Creep);
    // let target: Creep;
    // for (let i = 0; i < creeps.length; i++)
    // {
    //     target = creeps[i];
    //     if (!target.my)
    //         break;
    // }
    //
    // let towers = getObjectsByPrototype(StructureTower);
    // for (let i = 0; i < towers.length; i++)
    // {
    //     let tower = towers[i];
    //     if (!tower.my)
    //         continue;
    //
    //     // @ts-ignore
    //     console.log(tower);
    //     // @ts-ignore
    //     console.log(target);
    //     // @ts-ignore
    //     console.log(tower.attack(target));
    // }

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


