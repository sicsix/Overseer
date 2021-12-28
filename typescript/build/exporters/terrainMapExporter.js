import { getTerrainAt } from "game/utils";
import { ExportFlags } from "../enums";
const TERRAIN_WIDTH = 100;
const TERRAIN_HEIGHT = 100;
export class TerrainMapExporter {
    buf;
    constructor(exportBuffer) {
        this.buf = exportBuffer;
    }
    Run() {
        // TERRAIN CLEAR? = 0, TERRAIN WALL = 1, TERRAIN SWAMP = 2 - nav map to be processed from this + roads/structures on C++ side
        this.buf.WriteTypeHeader(ExportFlags.MAP, TERRAIN_WIDTH * TERRAIN_HEIGHT);
        for (let y = 0; y < TERRAIN_HEIGHT; y++) {
            for (let x = 0; x < TERRAIN_WIDTH; x++) {
                this.buf.WriteVal(getTerrainAt({ x: x, y: y }));
            }
        }
    }
}
//# sourceMappingURL=terrainMapExporter.js.map