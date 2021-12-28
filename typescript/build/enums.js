export var CommandType;
(function (CommandType) {
    CommandType[CommandType["START"] = -1] = "START";
    CommandType[CommandType["END"] = -2] = "END";
    CommandType[CommandType["INVALID"] = 0] = "INVALID";
    CommandType[CommandType["GAME_VISUAL_RECT"] = 1] = "GAME_VISUAL_RECT";
    CommandType[CommandType["MOVE"] = 2] = "MOVE";
})(CommandType || (CommandType = {}));
export var ExportFlags;
(function (ExportFlags) {
    ExportFlags[ExportFlags["NONE"] = 1] = "NONE";
    ExportFlags[ExportFlags["BEGIN"] = 2] = "BEGIN";
    ExportFlags[ExportFlags["END"] = 4] = "END";
    ExportFlags[ExportFlags["NEXT"] = 8] = "NEXT";
    ExportFlags[ExportFlags["DESTROYED"] = 32] = "DESTROYED";
    ExportFlags[ExportFlags["MAP"] = 64] = "MAP";
    ExportFlags[ExportFlags["CREEP"] = 128] = "CREEP";
})(ExportFlags || (ExportFlags = {}));
export var Colour;
(function (Colour) {
    Colour[Colour["WHITE"] = 0] = "WHITE";
    Colour[Colour["RED"] = 1] = "RED";
    Colour[Colour["GREEN"] = 2] = "GREEN";
    Colour[Colour["BLUE"] = 3] = "BLUE";
})(Colour || (Colour = {}));
export const ColourLookup = {
    [Colour.WHITE]: "#ffffff", [Colour.RED]: "#ff0000", [Colour.GREEN]: "#00ff00", [Colour.BLUE]: "#0000ff"
};
// export enum BufferType
// {
//     NONE = 0,
//     INT_8 = 1,
//     UINT_8 = 2,
//     INT_16 = 3,
//     UINT_16 = 4,
//     INT_32 = 5,
//     UINT_32 = 6,
//     FLOAT_32 = 7,
//     FLOAT_64 = 8,
// }
// export const TypeByteLength: number[] = [0, 1, 1, 2, 2, 4, 4, 4, 8];
//# sourceMappingURL=enums.js.map