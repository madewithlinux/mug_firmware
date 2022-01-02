#!/usr/bin/env node
// https://github.com/lukeed/sirv/pull/32#issuecomment-653462051

const { createProxyMiddleware } = require('http-proxy-middleware');
const polka = require('polka');
const sirv = require('sirv');

const { API_BASE_URL = 'http://192.168.1.173/', PORT = 3000, ROLLUP_WATCH } = process.env;
const dev = !!ROLLUP_WATCH;

polka()
  .use(
    createProxyMiddleware('/api', { target: API_BASE_URL, changeOrigin: true }),
    sirv('dist', { dev, single: true })
  )
  .listen(PORT, (err) => {
    if (err) throw err;
    console.log(`> Ready on localhost:${PORT}~!`);
  });
