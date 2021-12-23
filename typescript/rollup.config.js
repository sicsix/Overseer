// noinspection JSUnusedGlobalSymbols

import {emptyDir} from 'rollup-plugin-empty-dir';
import typescript from 'rollup-plugin-typescript2';
import {nodeResolve} from '@rollup/plugin-node-resolve';
import commonjs from '@rollup/plugin-commonjs';

export default {
  input: './main.ts',

  output: {
    dir: '../dist',
    entryFileNames: '[name].mjs',
    sourcemap: false,
  },

  external : ['game', 'game/utils', 'game/prototypes', 'game/constants'],

  plugins: [
    emptyDir(),
    typescript({tsconfig: './tsconfig.json'}),
    nodeResolve(),
    commonjs(),
  ],
};
