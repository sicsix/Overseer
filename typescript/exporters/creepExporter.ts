import { getObjectsByPrototype } from "game/utils";
import { Creep } from "game/prototypes";
import { ExportBuffer } from "../buffers/exportBuffer";
import { ATTACK, BodyPartConstant, CARRY, HEAL, MOVE, RANGED_ATTACK, TOUGH, WORK } from "game/constants";
import { ExportFlags } from "../enums";

export class CreepExporter
{
    private readonly buf: ExportBuffer;
    private readonly bodyPartLookup: Map<BodyPartConstant, number> = new Map<BodyPartConstant, number>([[MOVE, 1], [WORK, 2], [CARRY, 3], [ATTACK, 4], [RANGED_ATTACK, 5], [TOUGH, 6], [HEAL, 7]]);

    constructor(exportBuffer: ExportBuffer)
    {
        this.buf = exportBuffer;
    }

    Run()
    {
        let creeps = getObjectsByPrototype(Creep);

        if (creeps.length === 0) return;
        // TODO could potentially cache the previous creep list, iterate over them and check for creep.exists?

        this.buf.WriteTypeHeader(ExportFlags.CREEP, creeps.length);

        for (let i = 0; i < creeps.length; i++)
        {
            let creep = creeps[i];

            this.buf.NewObject();
            this.buf.WriteVal(Number(creep.id));
            this.buf.WriteVal(Number(creep.my));
            this.buf.WriteVal(creep.x);
            this.buf.WriteVal(creep.y);
            this.buf.WriteVal(creep.hits);
            this.buf.WriteVal(creep.hitsMax);
            this.buf.WriteVal(creep.body.length);

            // Packing the body parts 8 bits for type, 8 bits for hits, two body parts per int (1 int32 carries 2 body parts vs using 4 int32s without packing)
            if (creep.body.length > 1)
            {
                for (let j = 0; j < creep.body.length - 1; j += 2)
                {
                    let body1 = creep.body[j];
                    let body2 = creep.body[j + 1];
                    this.buf.WriteVal((this.bodyPartLookup.get(body1.type) as number) << 24 | body1.hits << 16 | (this.bodyPartLookup.get(body2.type) as number) << 8 | body2.hits);
                }
            }

            if (creep.body.length % 2 == 1)
            {
                let body = creep.body[creep.body.length - 1];
                this.buf.WriteVal((this.bodyPartLookup.get(body.type) as number) << 24 | body.hits << 16);
            }

            // TODO Add store later
        }

        // TODO i think the solution is to add an Valid component to entity, flip it to false at end of every frame, flip it to true when Adding/Updating entities
    }
}