//
// Created by Tim on 23/12/2021.
//

#ifndef OVERSEER_WASM_SRC_COMMANDS_COMMANDHANDLER_H_
#define OVERSEER_WASM_SRC_COMMANDS_COMMANDHANDLER_H_

#include "Commands.h"

namespace Overseer::Commands
{

    class CommandHandler
    {
      private:
        inline static double* Buffer;
        inline static int     CurrentOffset    = 0;
        inline static int     CommandCount     = 0;
        static const int      MaxCommandLength = 16384;

        enum CommandType
        {
            INVALID          = 0,
            GAME_VISUAL_RECT = 1,
            MOVE             = 2
        };

      public:
        static void SetBuffer(double* buffer)
        {
            Buffer = buffer;
        }

        static void Begin()
        {
            Buffer[0]     = -1;
            CurrentOffset = 1;
            CommandCount  = 0;
        }

        static int End()
        {
            Buffer[CurrentOffset] = -2;
            return CommandCount;
        }

        static void Add(Move move)
        {
            Add(MOVE, &move, sizeof(Move));
        }

        static void Add(GameVisualRect gameVisualRect)
        {
            Add(GAME_VISUAL_RECT, &gameVisualRect, sizeof(GameVisualRect));
        }

      private:
        static bool IsBufferFull(int commandSize)
        {
            return (CurrentOffset + commandSize + 1) >= MaxCommandLength;
        }

        static void Add(CommandType commandType, void* ptr, int bytes)
        {
            auto count = bytes / 8;

            if (IsBufferFull(count))
            {
                printf("[WASM] Command buffer full, skipping command...\n");
                return;
            }

            Buffer[CurrentOffset++] = (double)commandType;
            Buffer[CurrentOffset++] = (double)(count);

            memcpy(Buffer + CurrentOffset, ptr, bytes);

            CurrentOffset += count;
            CommandCount++;
        }
    };
} // namespace Overseer::Commands

using namespace Overseer::Commands;

#endif //OVERSEER_WASM_SRC_COMMANDS_COMMANDHANDLER_H_
