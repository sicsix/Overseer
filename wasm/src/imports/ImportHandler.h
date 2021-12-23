#ifndef OVERSEER_WASM_SRC_IMPORTS_IMPORTHANDLER_H_
#define OVERSEER_WASM_SRC_IMPORTS_IMPORTHANDLER_H_
#include "Includes.h"
#include "entt.hpp"
#include "ExportFlags.h"

namespace Overseer::Imports
{
    using ExportFlags = ExportFlagsNS::ExportFlags;

    class ImportHandler
    {
      public:
        static void Process(entt::registry& registry, uint* buffer, int objectCount);

      private:
        static void ProcessExport(entt::registry& registry,
                                  uint*           buffer,
                                  int&            currIndex,
                                  int&            curObject,
                                  ExportFlags     exportFlags);

        static void ImportTerrainMap(entt::registry& registry, uint* buffer, int& currIndex, int& curObject);

        static void ImportCreeps(entt::registry& registry, uint* buffer, int& currIndex, int& curObject);
    };
} // namespace Overseer::Imports

#endif //OVERSEER_WASM_SRC_IMPORTS_IMPORTHANDLER_H_
