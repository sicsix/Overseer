//
// Created by Tim on 26/12/2021.
//

#ifndef OVERSEER_WASM_SRC_COMMON_STRUCTURES_H_
#define OVERSEER_WASM_SRC_COMMON_STRUCTURES_H_
#include "Includes.h"

namespace Overseer::Common
{
	struct TerrainMap
	{
		int* Map;

		TerrainMap()
		{
		}

		TerrainMap(int* map) : Map(map)
		{
		}
	};

	struct NavMap
	{
		int* Map;

		NavMap()
		{
		}

		NavMap(int* map) : Map(map)
		{
		}
	};

}

#endif //OVERSEER_WASM_SRC_COMMON_STRUCTURES_H_
