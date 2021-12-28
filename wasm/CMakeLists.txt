cmake_minimum_required(VERSION 3.21)
project(Overseer)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_BUILD_TYPE Release)

include_directories(lib/linalg lib/entt/src/entt src)

set(TARGET_NAME module)

add_executable(${TARGET_NAME} src/Main.cpp src/Main.h src/imports/ImportHandler.h src/imports/ImportHandler.cpp src/imports/ExportFlags.h src/commands/CommandHandler.h src/commands/Commands.h src/commands/Visual.h src/components/Components.h src/common/Structures.h src/systems/movement/MovementSystem.h src/Includes.h src/systems/influence/InfluenceSystem.h src/systems/SystemBase.h src/systems/influence/InfluenceMaps.h src/common/Pathfinder.cpp src/common/Pathfinder.h src/common/Math.h src/systems/map/MapSystem.h src/Signals.h src/common/Navigation.h src/common/PriorityQueue.h src/common/Pathcoster.cpp src/common/Pathcoster.h)

# -s LLD_REPORT_UNDEFINED -Rpass=.* -O3 -msimd128 -msse4.1

# TODO Split the linker settings and compilation settings apart to avoid the warnings during compilation
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -msimd128 -msse4.1 -ffast-math -Wno-unused-command-line-argument  -s INITIAL_MEMORY=16777216 -s USE_ES6_IMPORT_META=0 -s INVOKE_RUN=1 -s EXIT_RUNTIME=0 -s FILESYSTEM=0 -s ENVIRONMENT=shell -s EXPORTED_FUNCTIONS=\"['_Loop', '_Initialise', '_malloc', '_free']\" -s INCOMING_MODULE_JS_API=\"['ENVIRONMENT', 'INITIAL_MEMORY', 'arguments', 'buffer', 'canvas', 'dynamicLibraries', 'extraStackTrace', 'instantiateWasm', 'mem', 'monitorRunDependencies', 'noExitRuntime', 'noInitialRun', 'onAbort', 'onCustomMessage', 'onExit', 'onFree', 'onMalloc', 'onRealloc', 'postMainLoop', 'postRun', 'preInit', 'preMainLoop', 'preRun', 'memoryInitializerRequest', 'preloadPlugins', 'print', 'printErr', 'quit', 'setStatus', 'statusMessage', 'stderr', 'stdin', 'stdout', 'thisProgram', 'wasm', 'wasmBinary']\" -s MODULARIZE=1 -s EXPORT_ES6=1 -s STRICT=1 -s SINGLE_FILE=1 -s FETCH_SUPPORT_INDEXEDDB=0 -s DYNAMIC_EXECUTION=0 -s WASM_ASYNC_COMPILATION=0 -s EXPORT_NAME=\"Module\"")

#add_compile_options("SHELL:-Rpass=.* -c -msimd128 -msse2 --no-entry")

#add_link_options("SHELL:-s INITIAL_MEMORY=16777216 -s USE_ES6_IMPORT_META=0 -s INVOKE_RUN=1 -s EXIT_RUNTIME=0 -s FILESYSTEM=0 -s ENVIRONMENT=shell -s EXPORTED_FUNCTIONS=\"['_Loop', '_malloc', '_free']\" -s INCOMING_MODULE_JS_API=\"['ENVIRONMENT', 'INITIAL_MEMORY', 'arguments', 'buffer', 'canvas', 'dynamicLibraries', 'extraStackTrace', 'instantiateWasm', 'mem', 'monitorRunDependencies', 'noExitRuntime', 'noInitialRun', 'onAbort', 'onCustomMessage', 'onExit', 'onFree', 'onMalloc', 'onRealloc', 'postMainLoop', 'postRun', 'preInit', 'preMainLoop', 'preRun', 'memoryInitializerRequest', 'preloadPlugins', 'print', 'printErr', 'quit', 'setStatus', 'statusMessage', 'stderr', 'stdin', 'stdout', 'thisProgram', 'wasm', 'wasmBinary']\" -s MODULARIZE=1 -s EXPORT_ES6=1 -s STRICT=1 -s SINGLE_FILE=1 -s FETCH_SUPPORT_INDEXEDDB=0 -s DYNAMIC_EXECUTION=0 -s WASM_ASYNC_COMPILATION=0 -s EXPORT_NAME=\"Module\"")

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/${TARGET_NAME}.js ${CMAKE_SOURCE_DIR}/build/${TARGET_NAME}.js)