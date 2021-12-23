/* eslint-disable camelcase */

declare module 'game/visual' {
  import {RoomPosition} from 'game/prototypes';

  export function rect(
    pos: RoomPosition,
    w: number,
    h: number,
    style?: Style
  ): void;

  export interface Style {
    fill?: string;
    opacity?: number;
    strokeWidth?: number;
    lineStyle?: string | undefined;
  }
}
