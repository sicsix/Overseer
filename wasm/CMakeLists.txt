cmake_minimum_required(VERSION 3.21)
project(Overseer)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_BUILD_TYPE Release)

include_directories(lib/linalg lib/entt/src/entt src lib/random/include)

set(TARGET_NAME module)

add_executable(${TARGET_NAME} src/Main.cpp src/Main.h src/imports/ImportHandler.h src/imports/ImportHandler.cpp src/imports/ExportFlags.h src/commands/CommandHandler.h src/commands/Commands.h src/commands/Visual.h src/components/Components.h src/core/Structures.h src/systems/movement/MovementSystem.h src/Includes.h src/systems/influence/InfluenceSystem.h src/systems/SystemBase.h src/core/Pathfinder.cpp src/core/Pathfinder.h src/core/Math.h src/systems/map/MapSystem.h src/Signals.h src/core/Navigation.h src/core/PriorityQueue.h src/core/MovementCoster.cpp src/core/MovementCoster.h src/core/LineOfSight.cpp src/core/LineOfSight.h src/core/Influence/InfluencePrecomputes.h src/core/Influence/InterestMap.h src/systems/ai/Squad.h src/systems/ai/GruntSystem.h src/systems/ai/SquadSystem.h src/core/Utility.h src/core/Influence/IMAP.h src/core/Influence/ProxIMAP.h src/core/Influence/ThreatIMAP.h src/core/Influence/LocalMap.h src/core/Influence/CreepProx.h src/core/Influence/CreepThreat.h src/core/Influence/CreepMovement.h src/core/Influence/Influence.h)

# DEBUG STUFF -s SAFE_HEAP=1 -s ASSERTIONS=1 -s STACK_OVERFLOW_CHECK=2 -s NO_DISABLE_EXCEPTION_CATCHING

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -msimd128 -msse4.1 -ffast-math -Wno-unused-command-line-argument  -s INITIAL_MEMORY=16777216 -s USE_ES6_IMPORT_META=0 -s INVOKE_RUN=1 -s EXIT_RUNTIME=0 -s FILESYSTEM=0 -s ENVIRONMENT=shell -s EXPORTED_FUNCTIONS=\"['_Loop', '_Initialise', '_malloc', '_free']\" -s INCOMING_MODULE_JS_API=\"['ENVIRONMENT', 'INITIAL_MEMORY', 'arguments', 'buffer', 'canvas', 'dynamicLibraries', 'extraStackTrace', 'instantiateWasm', 'mem', 'monitorRunDependencies', 'noExitRuntime', 'noInitialRun', 'onAbort', 'onCustomMessage', 'onExit', 'onFree', 'onMalloc', 'onRealloc', 'postMainLoop', 'postRun', 'preInit', 'preMainLoop', 'preRun', 'memoryInitializerRequest', 'preloadPlugins', 'print', 'printErr', 'quit', 'setStatus', 'statusMessage', 'stderr', 'stdin', 'stdout', 'thisProgram', 'wasm', 'wasmBinary']\" -s MODULARIZE=1 -s EXPORT_ES6=1 -s STRICT=1 -s SINGLE_FILE=1 -s FETCH_SUPPORT_INDEXEDDB=0 -s DYNAMIC_EXECUTION=0 -s WASM_ASYNC_COMPILATION=0 -s EXPORT_NAME=\"Module\"")

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/${TARGET_NAME}.js ${CMAKE_SOURCE_DIR}/build/${TARGET_NAME}.js)