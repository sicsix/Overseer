import { ColourLookup, CommandType } from "../enums";
import { assert } from "../assert";
import * as Game from "game";
import { getObjectById } from "game/utils";
export class CommandBuffer {
    module;
    maxCommandLength = 16384;
    ptr;
    commandBuffer;
    constructor(module) {
        this.module = module;
        this.ptr = module._malloc(this.maxCommandLength * 8);
        const start = this.ptr / 8;
        const end = start + this.maxCommandLength;
        this.commandBuffer = this.module.HEAPF64.subarray(start, end);
    }
    get Ptr() {
        return this.ptr;
    }
    Process(commandCount) {
        console.log(`[JS] Processing ${commandCount} commands...`);
        // console.log(this.commandBuffer);
        let currentIndex = 0;
        assert(this.commandBuffer[currentIndex++] === CommandType.START);
        for (let i = 0; i < commandCount; i++) {
            const commandType = this.commandBuffer[currentIndex++];
            const count = this.commandBuffer[currentIndex++];
            this.ExecuteCommand(commandType, count, currentIndex);
            currentIndex += count;
        }
        assert(this.commandBuffer[currentIndex] === CommandType.END);
    }
    ExecuteCommand(commandType, count, currentIndex) {
        switch (commandType) {
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
                throw "Invalid CommandType";
        }
    }
    Move(count, currentIndex) {
        assert(count === 2);
        const entity = this.commandBuffer[currentIndex++];
        const dir = this.commandBuffer[currentIndex++];
        const creep = getObjectById(entity);
        creep?.move(dir);
    }
    CommandGameVisualRect(count, currentIndex) {
        assert(count === 6);
        const x = this.commandBuffer[currentIndex++];
        const y = this.commandBuffer[currentIndex++];
        const sizeX = this.commandBuffer[currentIndex++];
        const sizeY = this.commandBuffer[currentIndex++];
        const colour = this.commandBuffer[currentIndex++];
        const opacity = this.commandBuffer[currentIndex++];
        Game.visual.rect({ x, y }, sizeX, sizeY, {
            fill: ColourLookup[colour], opacity: opacity
        });
    }
}
//# sourceMappingURL=commandBuffer.js.map