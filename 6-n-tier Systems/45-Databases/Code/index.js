//modules
var Engine = require('tingodb')();
var assert = require('assert');
var express = require('express');
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);

var db = new Engine.Db('./', {});

var Iconv  = require('iconv').Iconv,
    fs = require("fs");

var buffer = fs.readFileSync("smoke.txt"),
    iconv = new Iconv( "UTF-16", "UTF-8");

var array = iconv.convert(buffer).toString("utf8").split("\n");
// var fob_data ;
// var data;
// var data_for_database = [];
// var smoke;
// const net = require('net');
 var collection = db.collection("smoke_collection");
for (i = 1; i < array.length; i++){
  console.log(array[i]);
  var elements = array[i].split("\t");
  collection.insert([{time:parseFloat(elements[0]),ID:parseFloat(elements[1]),smoke:parseFloat(elements[2]),temp:parseFloat(elements[3])}], function(err, result) {
  assert.equal(null, err);
  collection.find().toArray(function(err,item){
          assert.equal(null, err);
          data = item;
        });
      });
};
// for (i in array){
//   var data_for_database = array[i].split("\t");
//   collection.insert([{time: parseFloat(data_for_database[0]),ID:parseFloat(data_for_database[1]),smoke:parseFloat(data_for_database[2]), temp:parseFloat(data_for_database[3])}], function(err,result){
//        assert.equal(null,err);
//        collection.find().toArray(function(err,item){
//          assert.equal(null, err);
//          data = item;
//        })
//  })
// });




// var array = fs.readFileSync('smoke.txt','utf8').toString().split("\n");
// // Fetch a collection to insert document into

//  }

// Points to index.html to serve webpage
app.get('/', function(req, res){
  res.sendFile(__dirname + '/index.html');
  //Points to stocks.txt
  app.get('/login.html', function(req, res) {
    res.sendFile(__dirname + '/login.html');
  });
});


// User socket connection
io.on('connection', function(socket){
  console.log('a user connected');
  //console.log(data[0]);
  io.emit('data_transmit', data);
  socket.on('disconnect', function(){
    console.log('user disconnected');
  });
});

// Listening on localhost:3000
http.listen(3000, function() {
  console.log('listening on *:3000');
});
