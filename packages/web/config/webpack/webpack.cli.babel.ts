import '../dotenv'

import path from 'path'

import ExtraWatchWebpackPlugin from 'extra-watch-webpack-plugin'
import webpack from 'webpack'

import { findModuleRoot } from '../../lib/findModuleRoot'
import { getEnvVars } from '../next/lib/getEnvVars'

import webpackIgnoreModules from './lib/webpackIgnoreModules'
import webpackLoadRaw from './lib/webpackLoadRaw'
import webpackLoadJavascript from './lib/webpackLoadJavascript'
import webpackTerser from './lib/webpackTerser'
import webpackTsChecker from './lib/webpackTsChecker'

import babelConfig from '../../babel.config'

const { NODE_ENV, ENABLE_SOURCE_MAPS, ENABLE_ESLINT, ENABLE_TYPE_CHECKS } = getEnvVars()

const MODE = NODE_ENV === 'development' ? 'development' : 'production'
const production = MODE === 'production'
const development = MODE === 'development'
const sourceMaps = ENABLE_SOURCE_MAPS

const { moduleRoot, pkg } = findModuleRoot()
const buildPath = path.join(moduleRoot, 'dist')
const outputFilename = 'nextclade.js' as const

function alias(development: boolean) {
  let productionAliases = {}

  if (development) {
    productionAliases = {
      ...productionAliases,
    }
  }

  return productionAliases
}

module.exports = {
  mode: MODE,
  bail: true,
  watch: true,
  name: pkg.name,
  target: 'node',
  devtool: 'cheap-module-source-map',
  stats: {
    all: false,
    errors: true,
    warnings: true,
    moduleTrace: true,
    colors: true,
  },
  performance: {
    hints: false,
  },

  entry: ['./src/cli/cli.ts'],

  output: {
    filename: outputFilename,
    path: path.join(buildPath),
    pathinfo: !development,
  },

  module: {
    rules: [
      ...webpackIgnoreModules({ patterns: /\.(css|sass|svg)$/ }),
      ...webpackLoadRaw({ patterns: /\.(txt|csv|tsv|fasta)$/i }),
      ...webpackLoadJavascript({ babelConfig, sourceMaps }),
    ],
  },

  resolve: {
    symlinks: false,
    extensions: ['.wasm', '.ts', '.tsx', '.mjs', '.es.js', '.js', '.jsx', '.json', '.yml'],
    alias: alias(development),
    modules: [moduleRoot, 'node_modules'],
  },

  node: false,

  plugins: [
    new ExtraWatchWebpackPlugin({
      files: [path.join(moduleRoot, 'src/types/**/*.d.ts')],
      dirs: [],
    }),

    webpackTsChecker({
      typeChecking: ENABLE_TYPE_CHECKS,
      eslint: ENABLE_ESLINT,
      memoryLimit: 2048,
    }),

    new webpack.optimize.LimitChunkCountPlugin({ maxChunks: 1 }),
  ].filter(Boolean),

  optimization: {
    nodeEnv: false,
    concatenateModules: false,
    noEmitOnErrors: true,
    occurrenceOrder: false,
    removeAvailableModules: true,
    removeEmptyChunks: true,
    minimize: production,
    runtimeChunk: false,
    splitChunks: false,
    minimizer: [production && webpackTerser({ sourceMaps, node: true, profile: false })].filter(Boolean),
  },
}
