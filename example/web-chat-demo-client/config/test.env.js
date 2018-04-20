'use strict'
const merge = require('webpack-merge')
const devEnv = require('./dev.env')

module.exports = merge(devEnv, {
  NODE_ENV: '"testing"',
  SERVER_URL: '"http://127.0.0.1:3000"',
  SOCKET_URL: '"ws://127.0.0.1:3000"'
})
