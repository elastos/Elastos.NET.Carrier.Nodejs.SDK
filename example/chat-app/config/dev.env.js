'use strict'
const merge = require('webpack-merge')
const prodEnv = require('./prod.env')

module.exports = merge(prodEnv, {
  NODE_ENV: '"development"',
  FLUENTFFMPEG_COV: '""',
  FFMPEG_PATH: '"/usr/local/bin/ffmpeg"'
})
