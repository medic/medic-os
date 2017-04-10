#!/usr/bin/env node

var http = require('http');

http.createServer(function(req, res) {
  res.writeHead(200, { 'X-Echo-Url':req.url });
  req.pipe(res);
}).listen(process.argv[2]);
