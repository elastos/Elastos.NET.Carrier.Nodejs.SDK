const path = require('path')
const url = require('url')
const fs = require('fs')
const http = require('http')
const exec = require('child_process').exec
const rootPath = path.resolve(__dirname, '../')
const nwPath = require('nw').findpath()

// 修改index.html文件中的css和js的地址
const indexHtmlPath = path.resolve(__dirname, '../index.html')
const indexHtmlContent = fs.readFileSync(indexHtmlPath, 'utf-8').toString()

// 退出时恢复原样子
process.on('exit', exitHandle)
process.on('uncaughtException', exitHandle)

function exitHandle(e) {
  fs.writeFileSync(indexHtmlPath, indexHtmlContent, 'utf-8')
  console.log('233333,bye~~~')
}

// get uri
var config = require('../config')
if (!process.env.NODE_ENV) process.env.NODE_ENV = JSON.parse(config.dev.env.NODE_ENV)
var port = process.env.PORT || config.dev.port
var uri = `http://localhost:${port}/`

// start lauch NW.js
requestGet(uri, htmlText => {
  htmlText = htmlText.replace('src="/', `src="${uri}`).replace('href="/', `href="${uri}`)
  fs.writeFileSync(indexHtmlPath, htmlText, 'utf-8')

  let runNwDev = exec(`${nwPath} ./`, { cwd: rootPath }, (err, stdout, stderr) => {
    if (err) process.exit(0)
  })

  runNwDev.stdout.on('data', (data) => console.log(data))
})

function requestGet(path, callback) {
  console.log('start with request: ', path)
  const options = Object.assign({ method: 'GET' }, url.parse(path))
  const req = http.request(options, res => {
    let body = []
    res.on('data', chunk => body.push(chunk))
    res.on('end', () => callback(Buffer.concat(body).toString()))
  })
  req.on('error', e => console.log('problem with request: ' + e.message))
  req.write('')
  req.end()
}


