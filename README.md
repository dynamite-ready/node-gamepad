Node Gamepad v0.1.0

A cross platform, native module for gamepads, based on SDL 2.
Presently works on Windows and Mac OSX.

A Linux version is in the works. I'll endeavour to provide more detail before long, but the basic instructions for use are as follows:

Installation:
Make sure you have Node JS installed, obviously. Node-Gyp is essential too.
Copy the repo to you machine. Using the command terminal, 'cd' into the repo/module and run:

node-gyp configure
node-gyp build

After doing so, you should be able to use the methods found in index.js.
NPM support coming soon.

Basic Usage:
The module makes use of Node JS's event system. For a simple demonstration, set up a simple node project, create an index.js file, and in it, add:

var Gamepad = require("node-gamepad");

var myGamepad = new Gamepad(0);

console.log("name: ", myGamepad.name);

global.setInterval(function(){
    myGamepad.events.on("xaxis", function(direction){ console.log("X-Axis: ", direction); });
    myGamepad.events.on("yaxis", function(direction){ console.log("Y-Axis: ", direction); });
    _myGamepad.events.on("buttondown", function(button){ console.log("Button: ", direction); })
}, 200);