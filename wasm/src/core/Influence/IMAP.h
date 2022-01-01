//
// Created by Tim on 1/01/2022.
//

#ifndef OVERSEER_WASM_SRC_CORE_IMAP_H_
#define OVERSEER_WASM_SRC_CORE_IMAP_H_
#include "Includes.h"
#include "commands/CommandHandler.h"

namespace Overseer::Core::Influence
{
    struct IMAP
    {
        float* Influence = new float[MAP_SIZE];

        void Clear()
        {
            memset(Influence, 0, sizeof(float) * MAP_SIZE);
        }

        void DebugDraw(float minVal, int maxCount)
        {
            int count = 0;
            for (int i = 0; i < MAP_SIZE; ++i)
            {
                float influence = Influence[i];

                auto pos = double2(i % MAP_WIDTH, i / MAP_WIDTH);

                if (influence > minVal)
                {
                    count++;
                    CommandHandler::Add(GameVisualRect(pos, { 1.0, 1.0 }, Colour::RED, min(influence, 1.0f)));
                }

                if (count >= maxCount)
                {
                    printf("[WASM] InfluenceMap debug draw limit of %i elements hit\n", maxCount);
                    break;
                }
            }
        }
    };
} // namespace Overseer::Core
#endif //OVERSEER_WASM_SRC_CORE_IMAP_H_
