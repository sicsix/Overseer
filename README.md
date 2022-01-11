# Overseer
A pre-alpha bot for the game [Screeps Arena](https://store.steampowered.com/app/1137320/Screeps_Arena/) written in Typescript and C++ in WebAssembly.

#### Features
* Core of the bot is written in C++ which is compiled to WebAssembly by the LLVM compiler, improving performance over using JavaScript alone
* JavaScript merely updates WASM with the current world state and has no AI logic
* Code written in a style to enable easy auto vectorisation by the LLVM compiler, gaining performance boost from SIMD instructions without explicitly writing SIMD instructions
* Only a single interop call from JS to WASM, which runs the main loop
* No interops between WASM and JS (not including debug/error prints)
* Game objects are written directly to WASM shared memory by JS as a stream of bytes, which is decoded in C++
* Decoded game objects converted to entities and kept updated in an Entity Component System - [entt](https://github.com/skypjack/entt)
* Highly optimised pathfinding - written to take advantage of SIMD instructions
* Influence Map system, capable of tracking threat and proximity of creeps utilising on the fly calculated movement costs and visibility checks in the creeps radius
* Interest Map system, allows for combining influence maps in various ways to allow the individual creeps to make decisions based on spatial information
* WASM runs AI and sends commands back to JS by writing to a stream of shared memory. JS decodes this stream and executes the commands in the game world

#### In Progess
* Much much more to be done...
