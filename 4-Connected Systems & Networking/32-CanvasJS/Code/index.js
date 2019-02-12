//Modules
var express = require('express');
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);

// Points to index.html to serve webpage
app.get('/', function(req, res){
  res.sendFile(__dirname + '/index.html');
  //Points to stocks.txt
  app.get('/data', function(req, res) {
    res.sendFile(__dirname + '/stocks.txt');
  });
});
// Read the file and print its contents.
var fs = require('fs')
  , filename = __dirname + '/stocks.txt';
var csv_data = fs.readFileSync(filename, 'utf8').toString().split("\n");
var data = [];
for (i=1;i<csv_data.length -1; i++) {
  data[i-1] = csv_data[i];
  console.log(data[i-1]);
}

// User socket connection
io.on('connection', function(socket){
  console.log('a user connected');
  io.emit('transmit_data', data);
  socket.on('disconnect', function(){
    console.log('user disconnected');
  });
});

// Listening on localhost:3000
http.listen(3000, function() {
  console.log('listening on *:3000');
});
