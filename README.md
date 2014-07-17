#Node Gamepad v0.1.0

A Node JS cross platform native module for gamepads, based on SDL 2 (https://www.libsdl.org/).
Presently works on Windows and Mac OSX. A Linux version is in the works. I'll also endeavour to provide more detail before long, but the basic instructions for use are as follows:

###Installation:
Make sure you have Node JS installed, obviously. Node-Gyp is essential too.
Copy the repo to your machine. Using the command terminal, 'cd' into the repo/module and run:

```
node-gyp configure
node-gyp build
```

After doing so, you should be able to use the methods found in index.js.
NPM support coming soon.

###Basic Usage:
The module makes use of Node JS's event system. For a simple demonstration, set up a simple node project, create an index.js file, and in it, add:

```javascript
var Gamepad = require("node-gamepad");

var myGamepad = new Gamepad(0);

console.log("name: ", myGamepad.name);

global.setInterval(function(){
    myGamepad.events.on("xaxis", function(direction){ console.log("X-Axis: ", direction); });
    myGamepad.events.on("yaxis", function(direction){ console.log("Y-Axis: ", direction); });
    _myGamepad.events.on("buttondown", function(button){ console.log("Button: ", direction); })
}, 200);
```

Also insure that you take the OS specific SDL runtime library file from the 'runtime-libraries' folder, and copy the file to the same root folder that you run 'node index.js' from.
