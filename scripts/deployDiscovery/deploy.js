#!/usr/bin/env node

var process = require('process');

// print process.argv
process.argv.forEach(function (val, index, array) {
  console.log(index + ': ' + val);
});