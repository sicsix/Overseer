import { WASMModule } from "../../wasm/build/module";
import { ExportFlags } from "../enums";
import { assert } from "../assert";

export class ExportBuffer
{
    private readonly module: WASMModule;
    private readonly size = 1048576; // 1 MB
    private readonly ptr: number;
    private readonly buf: Uint32Array;

    private curIndex = 0;
    private objectCount = 0;

    constructor(module: WASMModule)
    {
        this.module = module;
        this.ptr = module._malloc(this.size);

        let start = this.ptr / 4;
        let end = start + this.size / 4;
        this.buf = this.module.HEAPU32.subarray(start, end);
    }

    get Ptr(): number
    {
        return this.ptr;
    }

    Begin()
    {
        this.objectCount = 0;
        this.curIndex = 0;
        this.buf[this.curIndex++] = ExportFlags.BEGIN;
    }

    WriteTypeHeader(exportFlags: ExportFlags, count: number)
    {
        this.buf[this.curIndex++] = ExportFlags.NEXT | exportFlags;
        this.buf[this.curIndex++] = count;
    }

    NewObject()
    {
        this.objectCount++;
    }

    // WriteFlags(exportFlags: ExportFlags)
    // {
    //     this.buf[this.curIndex++] = exportFlags;
    //     this.objectCount++;
    // }

    WriteVal(val: number)
    {
        this.buf[this.curIndex++] = val;
    }

    End(): number
    {
        assert(this.curIndex * 4 < this.size, "Export buffer overrun");
        this.buf[this.curIndex] = ExportFlags.END;
        return this.objectCount;
    }
}
