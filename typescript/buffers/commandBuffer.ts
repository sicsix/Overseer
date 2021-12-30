import { WASMModule } from "../../wasm/build/module";
import { Colour, ColourLookup, CommandType } from "../enums";
import { assert } from "../assert";
import * as Game from "game";
import { getObjectById } from "game/utils";
import { Creep, Id } from "game/prototypes";
import { DirectionConstant } from "game/constants";

export class CommandBuffer
{
    private readonly module: WASMModule;
    private readonly maxCommandLength = 16384;
    private readonly ptr: number;
    private readonly commandBuffer;

    constructor(module: WASMModule)
    {
        this.module = module;
        this.ptr = module._malloc(this.maxCommandLength * 8);
        const start = this.ptr / 8;
        const end = start + this.maxCommandLength;
        this.commandBuffer = this.module.HEAPF64.subarray(start, end);
    }

    get Ptr(): number
    {
        return this.ptr;
    }

    Process(commandCount: number)
    {
        console.log(`[JS] Processing ${commandCount} commands...`);

        // console.log(this.commandBuffer);

        let currentIndex = 0;

        assert(this.commandBuffer[currentIndex++] === CommandType.START);

        for (let i = 0; i < commandCount; i++)
        {
            const commandType: CommandType = this.commandBuffer[currentIndex++];
            const count = this.commandBuffer[currentIndex++];
            this.ExecuteCommand(commandType, count, currentIndex);
            currentIndex += count;
        }

        assert(this.commandBuffer[currentIndex] === CommandType.END);
    }

    private ExecuteCommand(commandType: CommandType, count: number, currentIndex: number)
    {
        switch (commandType)
        {
            case CommandType.GAME_VISUAL_RECT:
                this.CommandGameVisualRect(count, currentIndex);
                break;
            case CommandType.MOVE:
                this.Move(count, currentIndex);
                break;
            case CommandType.START:
            case CommandType.END:
            case CommandType.INVALID:
            default:
                throw "[JS] ERROR: Invalid CommandType";
        }
    }

    private Move(count: number, currentIndex: number)
    {
        assert(count === 2);
        const entity = this.commandBuffer[currentIndex++];
        const dir = this.commandBuffer[currentIndex++] as DirectionConstant;

        const creep = getObjectById<Creep>(entity as unknown as Id<Creep>);

        creep?.move(dir);
    }

    private CommandGameVisualRect(count: number, currentIndex: number)
    {
        assert(count === 6);
        const x = this.commandBuffer[currentIndex++];
        const y = this.commandBuffer[currentIndex++];
        const sizeX = this.commandBuffer[currentIndex++];
        const sizeY = this.commandBuffer[currentIndex++];
        const colour: Colour = this.commandBuffer[currentIndex++];
        const opacity = this.commandBuffer[currentIndex++];

        Game.visual.rect({ x, y }, sizeX, sizeY, {
            fill: ColourLookup[colour], opacity: opacity
        });
    }
}
