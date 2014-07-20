var nodeGamepad = require("./build/Release/nodeGamepad");
var e = require("events");

// Separate file for this, most likely.
var joypadConfig = { 
	ps3: {
		dpad: [
			// Button numbers.
			{ number: 4, action: -1, axis: "yaxis" },
			{ number: 5, action: 1, axis: "xaxis" },
			{ number: 6, action: 1, axis: "yaxis" },
			{ number: 7, action: -1, axis: "xaxis" }
		]
	},
	
	xbox360: {
		dpad: [
			// Button numbers.
			{ number: 0, action: -1, axis: "yaxis" },
			{ number: 1, action: 1, axis: "yaxis" },
			{ number: 2, action: -1, axis: "xaxis" },
			{ number: 3, action: 1, axis: "xaxis" }
		],
		
		triggers: [ 4, 5 ] // Axes... AKA analogue buttons.
	}
};

// Constructor...
function Device(deviceID, deadzone){
	var _self = this;
	this.events = new e.EventEmitter();
	this.deviceID = deviceID;
	this.dirValueSet = [];
	
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
		this.dirValueSet = []
		if(this.device && nodeGamepad.deviceCount >= Number(this.deviceID + 1)){
			this.pollButtons(this.device.getButtonStateArray());
			this.pollAxes(this.device.getAxisStateArray());
			if(callback) callback();
			// console.log(this.device.getButtonStateArray())
			// console.log(this.device.getAxisStateArray());
		}
	},
	
	pollButtons: function(stateArray){
		var _self = this;
		
		if(this.device && nodeGamepad.deviceCount >= Number(this.deviceID + 1)){
			for(var button in stateArray){
				if(stateArray[button]){
					switch(this.name){
						case "PLAYSTATION(R)3 Controller":
							dpad(joypadConfig.ps3.dpad, button);
							break;
						case "XInput Controller #1":
							dpad(joypadConfig.xbox360.dpad, button);
							break;
						default:
							this.events.emit("buttondown", Number(button + 1));
							break;
					}
				};
			};	
		}
		
		function dpad(map, button){
			var isDpad = false;
			for(var dbutton in map){
				if(button == map[dbutton]["number"]){
					isDpad = true;
					// Adequate, but this needs to be cleaner.
					if(map[dbutton]["axis"] == "yaxis"){
						if(!_self.dirValueSet[1])
							_self.events.emit("yaxis", map[dbutton]["action"]);
						
						_self.dirValueSet[1] = true;
					} else {
						if(!_self.dirValueSet[0]) 
							_self.events.emit("xaxis", map[dbutton]["action"]);

						_self.dirValueSet[0] = true;
					}
				}
			};
			
			if(!isDpad)
				_self.events.emit("buttondown", Number(button + 1));			
		}	
	},
	
	pollAxes: function(stateArray){
		var _self = this;
		// Any even number (0, 2, 4) will be assumed to address the X axis,
		// and odd numbers, Y in this implementation. Dicking about with the successive axes
		// will cancel out the preceding pair.
		
		if(this.device && nodeGamepad.deviceCount >= Number(this.deviceID + 1)){
			for(var axis = 0; axis < stateArray.length; axis++){
				var axisValue = 0;
				if(stateArray[axis] != 0){	
					// Calculate the joystick deadzone...
					if(this.deadzone){
						var evaluateDeadzone = (
							stateArray[axis] < Number(this.deadzone) && 
							stateArray[axis] > -Number(this.deadzone)
						);
					}
						
					if(!evaluateDeadzone){				
						if(stateArray[axis] > 0) axisValue = 1;
						if(stateArray[axis] < 0) axisValue = -1;
					}
					
					if(this.name == "XInput Controller #1"){
						if(
							axis == joypadConfig.xbox360.triggers[0] ||
							axis == joypadConfig.xbox360.triggers[1]
						){
							if(axisValue >= 1){
								if(axis == joypadConfig.xbox360.triggers[0]){
									this.events.emit("buttondown", "L");
								}
								
								if(axis == joypadConfig.xbox360.triggers[1]){
									this.events.emit("buttondown", "R");
								}
							} 
						} else emitAxisEvent();
					} else emitAxisEvent();
					
					function emitAxisEvent(){
						if(Number(axis % 2) == 0){
							if(!_self.dirValueSet[0] && Number(axisValue)) 
								_self.events.emit("xaxis", Number(axisValue));
							if(Number(axisValue)) _self.dirValueSet[0] = true;
							//console.log("XAXIS: ", axisValue);
						} else {
							if(!_self.dirValueSet[1] && Number(axisValue)) 
								_self.events.emit("yaxis", Number(axisValue));
							if(Number(axisValue)) _self.dirValueSet[1] = true;
							//console.log("YAXIS: ", axisValue);
						}
					}
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
		
		if(this.device) this.device.close();
		if(deviceShouldClose) delete this;
		if(callback) callback();
	}
};

module.exports = Device;
