//
// Created by Tim on 23/12/2021.
//

#ifndef OVERSEER_WASM_SRC_COMMANDS_COMMANDS_H_
#define OVERSEER_WASM_SRC_COMMANDS_COMMANDS_H_

#include "Includes.h"
#include "Visual.h"

namespace Overseer::Commands
{
    enum Direction
    {
        TOP          = 1,
        TOP_RIGHT    = 2,
        RIGHT        = 3,
        BOTTOM_RIGHT = 4,
        BOTTOM       = 5,
        BOTTOM_LEFT  = 6,
        LEFT         = 7,
        TOP_LEFT     = 8
    };

    struct Move
    {
        double Entity;
        double Dir;

        Move(double entity, double dir): Entity(entity), Dir(dir)
        {
        }
    };

    struct GameVisualRect
    {
        double2 Pos;
        double2 Size;
        double  Colour;
        double  Opacity;

        GameVisualRect(double2 pos, double2 size, Overseer::Commands::Colour colour, double opacity)
        {
            GameVisualRect::Pos     = pos - 0.5f;
            GameVisualRect::Size    = size;
            GameVisualRect::Colour  = colour;
            GameVisualRect::Opacity = opacity;
        }
    };
} // namespace Overseer::Commands

#endif //OVERSEER_WASM_SRC_COMMANDS_COMMANDS_H_
