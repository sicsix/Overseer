import Module, { WASMModule } from "../wasm/build/module";
import { ExportBuffer } from "./buffers/exportBuffer";
import { CommandBuffer } from "./buffers/commandBuffer";
import { assert } from "./assert";

export class ModuleManager
{
    private readonly module: WASMModule;
    private readonly exportBuffer: ExportBuffer;
    private readonly commandBuffer: CommandBuffer;

    constructor()
    {
        const opts: any = {};
        opts.print = opts.print || ((text: string) => console.log(text));
        opts.printErr = opts.printErr || ((text: string) => console.log(`[JS] ERROR: ${text}`));
        opts.onAbort = opts.onAbort || (() => console.log("[JS] FATAL: WASM ABORTED"));
        this.module = Module(opts);

        this.exportBuffer = new ExportBuffer(this.module);
        this.commandBuffer = new CommandBuffer(this.module);
    }

    get ExportBuffer(): ExportBuffer
    {
        return this.exportBuffer;
    }

    Initialise()
    {
        assert(this.module._Initialise(this.exportBuffer.Ptr, this.commandBuffer.Ptr));
        this.Begin();
    }

    Begin()
    {
        this.exportBuffer.Begin();
    }

    Execute()
    {
        let exportCount = this.exportBuffer.End();
        console.log(`[JS] Executing WASM...`);
        const commandCount = this.module._Loop(exportCount);
        this.commandBuffer.Process(commandCount);
    }
}
