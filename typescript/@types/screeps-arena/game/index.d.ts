/// <reference path="utils.d.ts" />
/// <reference path="path-finder.d.ts" />
/// <reference path="constants.d.ts" />
/// <reference path="prototypes/index.d.ts" />
/// <reference path="visual.d.ts" />

declare module 'game' {
  import {
    ERR_BUSY,
    ERR_INVALID_ARGS,
    ERR_NOT_ENOUGH_ENERGY,
  } from 'game/constants';
  import {Structure} from 'game/prototypes';

  export * as utils from 'game/utils';
  export * as pathFinder from 'game/path-finder';

  export * as prototypes from 'game/prototypes';

  export * as constants from 'game/constants';

  export * as visual from 'game/visual';

  export const arenaInfo: {
    /**
     * "Capture the Flag"
     */
    name: string;
    level: number;
    /**
     * "alpha"
     */
    season: string;
    ticksLimit: number;
    cpuTimeLimit: number;
    cpuTimeLimitFirstTick: number;
  };

  export function createConstructionSite(
    x: number,
    y: number,
    structurePrototype: string /*STRUCTURE_PROTOTYPES*/
  ): {
    object?: Structure | undefined;
    error?: ERR_BUSY | ERR_INVALID_ARGS | ERR_NOT_ENOUGH_ENERGY | undefined;
  };
}
