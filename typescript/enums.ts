export enum CommandType
{
    START = -1,
    END = -2,
    INVALID = 0,
    GAME_VISUAL_RECT = 1,
    MOVE = 2,
}

export enum ExportFlags
{
    NONE = 1 << 0,
    BEGIN = 1 << 1,
    END = 1 << 2,
    NEXT = 1 << 3, // NEW = 1 << 4,
    DESTROYED = 1 << 5,
    MAP = 1 << 6,
    CREEP = 1 << 7
}

export enum Colour
{
    WHITE,
    RED,
    GREEN,
    BLUE
}

export const ColourLookup: Record<Colour, string> = {
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


