export interface WASMModule extends EmscriptenModule {
    _Initialise(messagePtr: number, commandPtr: number): boolean;
    _Loop(count: number): number;
}

export default function Module(mod?: any): WASMModule;