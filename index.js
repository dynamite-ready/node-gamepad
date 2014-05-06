var nodeGamepad = require("./build/Release/nodeGamepad");
var e = require("events");

// Constructor...
function Device(deviceID, deadzone){
	var _self = this;
	this.events = new e.EventEmitter();
	this.deviceID = deviceID;
	try {	
		if(nodeGamepad.deviceCount >= Number(this.deviceID + 1))
			this.device = new nodeGamepad.Gamepad(this.deviceID || 0);
		
		if(this.device){
			this.name = this.device.name
			this.buttons = this.device.buttons;
			this.axes = this.device.axes;
			if(deadzone) this.deadzone = deadzone;
			this.events.emit("plugged", Number(this.deviceID));
		} else {
			this.events.emit("unplugged", Number(this.deviceID));
		}
	} catch(e){
		this.events.emit("unplugged", Number(this.deviceID));
	}
}

Device.prototype = {
	checkDevice: function(){
		if(this.device && nodeGamepad.deviceCount >= Number(this.deviceID + 1)){
			var deviceStatus = false;
			deviceStatus = this.device.getAttachedStatus();
			deviceStatus?
				this.events.emit("plugged", Number(this.deviceID)):
				this.events.emit("unplugged", Number(this.deviceID));
		} else {
			this.events.emit("unplugged", Number(this.deviceID));
		}
	},
	
	pollDevice: function(callback){
		if(this.device && nodeGamepad.deviceCount >= Number(this.deviceID + 1)){
			this.pollButtons(this.device.getButtonStateArray());
			this.pollAxes(this.device.getAxisStateArray());
			if(callback) callback();
		}
	},
	
	pollButtons: function(stateArray){
		if(this.device && nodeGamepad.deviceCount >= Number(this.deviceID + 1)){
			for(var button in stateArray){
				if(stateArray[button]){
					this.events.emit("buttondown", Number(button + 1));
				};
			};	
		}
	},
	
	pollAxes: function(stateArray){
		// Any even number (0, 2, 4) will be assumed to address the X axis,
		// and odd numbers, Y in this implementation. Dicking about with the successive axes
		// will cancel out the preceding pair.
		if(this.device && nodeGamepad.deviceCount >= Number(this.deviceID + 1)){
			for(var axis in stateArray){
				var axisValue = 0;
				if(stateArray[axis] != 0){
					// Calculate the joystick deadzone...
					if(this.deadzone){
						if(stateArray[axis] < Number(this.deadzone) && stateArray[axis] > -Number(this.deadzone))
							stateArray[axis] = 0;
					}
					if(stateArray[axis] > 0) axisValue = 1;
					if(stateArray[axis] < 0) axisValue = -1;
					
					axis == 0 || (axis % 2) == 0 ?
						this.events.emit("xaxis", Number(axisValue)):
						this.events.emit("yaxis", Number(axisValue));
				}
			}
		}
	},
	
	close: function(callback){
		var _self = this;
		var deviceShouldClose = (
			nodeGamepad.deviceCount < Number(this.deviceID + 1) ||
			nodeGamepad.deviceCount == 0
		);
		
		if(this.device) if(this.device) this.device.close();
		if(deviceShouldClose) delete this;
		if(callback) callback();
	}
};

/*
global.setInterval(function(){
	// Check for signs of life.
	_self.gamepad.checkDevice();
}, 1500);
var gamepad = new Device(0);
global.setInterval(function(){
	// Read the pad, render the frame.
	gamepad.checkDevice();
}, 17);		
*/


module.exports = Device;