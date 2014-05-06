#include <v8.h>
#include <nan.h>
#include <node.h>
#include <SDL.h>
#include <loadsdl.hpp> // Simple OS specific DLL management.
#include <node-gamepad.hpp>

using namespace v8;
using namespace node;

// The Javascript based constructor wrapper.
Persistent<FunctionTemplate> Gamepad::constructor;

// Native object constructor.
Gamepad::Gamepad(unsigned int deviceNumber){
	deviceIndex = 0; // Should insist on the right value.
	if(deviceNumber) deviceIndex = deviceNumber;
	object = SDL_JoystickOpen(deviceIndex);
	SDL_JoystickUpdate();
	if(object){
		axes = SDL_JoystickNumAxes(object);
		buttons = SDL_JoystickNumButtons(object);
		deviceName = SDL_JoystickName(object);
	}
}


Gamepad::~Gamepad(){
	// SDL_Quit();
}

NAN_METHOD(Gamepad::New){
	HandleScope scope;
    Gamepad* currentGamepad = new Gamepad(args[0] -> ToInteger() -> Value());
    currentGamepad -> Wrap(args.This());
	SDL_JoystickUpdate();
    NanReturnValue(args.This());
} 

NAN_METHOD(Gamepad::getButtonStateArray){
	NanScope();
	Gamepad* currentGamepad = ObjectWrap::Unwrap<Gamepad>(args.This());
	if(currentGamepad -> object){
		SDL_JoystickUpdate();
		Handle<Array> value = Array::New(currentGamepad -> buttons);
		for(unsigned int i = 0; i < currentGamepad -> buttons; i++)
			value -> Set(i, Integer::New(SDL_JoystickGetButton(currentGamepad -> object, i)));
		NanReturnValue(value);
	} else NanReturnUndefined();
}

// Get pad/stick input data.
NAN_METHOD(Gamepad::getAxisStateArray){
	NanScope();
	Gamepad* currentGamepad = ObjectWrap::Unwrap<Gamepad>(args.This());
	if(currentGamepad -> object){
		SDL_JoystickUpdate();
		Handle<Array> value = Array::New(currentGamepad -> axes);
		for(unsigned int i = 0; i < currentGamepad -> axes; i++)
			value -> Set(i, Number::New(SDL_JoystickGetAxis(currentGamepad -> object, i)));
		NanReturnValue(value);
	} else NanReturnUndefined();
}

// Find out if the pad is still connected.
NAN_METHOD(Gamepad::getAttachedStatus){
	NanScope();
	Gamepad* currentGamepad = ObjectWrap::Unwrap<Gamepad>(args.This());
	if(currentGamepad -> object){
		SDL_JoystickUpdate();
		SDL_bool connectionStatus = SDL_JoystickGetAttached(currentGamepad -> object);
		Handle<Boolean> value = Boolean::New(connectionStatus == SDL_TRUE ? true : false);
		NanReturnValue(value);
	} else NanReturnUndefined();
}

NAN_METHOD(Gamepad::closeDevice){
	NanScope();
	Gamepad* currentGamepad = ObjectWrap::Unwrap<Gamepad>(args.This());
	if(currentGamepad -> object)
		SDL_JoystickClose(currentGamepad -> object);
	
	NanReturnUndefined();
}

NAN_GETTER(Gamepad::getDeviceId){
	NanScope();
	Gamepad* currentGamepad = ObjectWrap::Unwrap<Gamepad>(args.This());
	Handle<Integer> value = Integer::New(currentGamepad -> deviceID);
	NanReturnValue(value);
}

NAN_GETTER(Gamepad::getDeviceName){
	NanScope();
	Gamepad* currentGamepad = ObjectWrap::Unwrap<Gamepad>(args.This());
	if(currentGamepad -> object){
		Handle<String> value = String::New(currentGamepad -> deviceName);
		NanReturnValue(value);
	} else NanReturnUndefined();
}

NAN_GETTER(Gamepad::getButtonCount){
	NanScope();
	Gamepad* currentGamepad = ObjectWrap::Unwrap<Gamepad>(args.This());
	if(currentGamepad -> object){
		Handle<Integer> value = Integer::New(currentGamepad -> buttons);
		NanReturnValue(value);
	} else NanReturnUndefined();
}

NAN_GETTER(Gamepad::getAxesCount){
	NanScope();
	Gamepad* currentGamepad = ObjectWrap::Unwrap<Gamepad>(args.This());
	if(currentGamepad -> object){
		Handle<Integer> value = Integer::New(currentGamepad -> axes);
		NanReturnValue(value);
	} else NanReturnUndefined();
}

NAN_GETTER(Gamepad::getDeviceCount){
	NanScope();
	SDL_JoystickUpdate();
	Handle<Integer> value = Integer::New(SDL_NumJoysticks());
	NanReturnValue(value);
}

NAN_GETTER(Gamepad::getDeviceIndex){
	NanScope();
	Gamepad* currentGamepad = ObjectWrap::Unwrap<Gamepad>(args.This());
	if(currentGamepad -> object){
		Handle<Integer> value = Integer::New(currentGamepad -> deviceIndex);
		NanReturnValue(value);
	} else NanReturnUndefined();
}

void Gamepad::initialise(Handle<Object> exports){
	NanScope();
	
	// Start the SDL joystick subsystem.
	SDL_Init(SDL_INIT_JOYSTICK);
	
	Local<String> name = NanSymbol("Gamepad");
	// Specify and create the Javascript based object constructor.
    Local<FunctionTemplate> constructorTemplate = FunctionTemplate::New(Gamepad::New);
	NanAssignPersistent(FunctionTemplate, constructor, constructorTemplate);
    constructorTemplate -> InstanceTemplate() -> SetInternalFieldCount(1); // Brr...
    constructorTemplate -> SetClassName(name);
	
	// Create the native object prototype.
	Local<ObjectTemplate> proto = constructorTemplate -> PrototypeTemplate();
	proto -> SetInternalFieldCount(1);
	
	// Prototypal properties.
	proto -> SetAccessor(NanSymbol("deviceId"), getDeviceId);
	proto -> SetAccessor(NanSymbol("deviceIndex"), getDeviceIndex);
	proto -> SetAccessor(NanSymbol("name"), getDeviceName);
	proto -> SetAccessor(NanSymbol("buttons"), getButtonCount);
	proto -> SetAccessor(NanSymbol("axes"), getAxesCount);
	
	// Prototype methods...
	proto -> Set(
		NanSymbol("getButtonStateArray"),
		FunctionTemplate::New(getButtonStateArray) -> GetFunction()
	);
	
	proto -> Set(
		NanSymbol("getAxisStateArray"),
		FunctionTemplate::New(getAxisStateArray) -> GetFunction()
	);

	proto -> Set(
		NanSymbol("getAttachedStatus"), 
		FunctionTemplate::New(getAttachedStatus) -> GetFunction()
	);
	
	proto -> Set(
		NanSymbol("close"), 
		FunctionTemplate::New(closeDevice) -> GetFunction()
	);	
	
	
	// To get the initial device count
	// before deciding to create a new object or not.
	exports -> SetAccessor(NanSymbol("deviceCount"), getDeviceCount);
	// Export the composed persistent container.
	exports -> Set(name, constructorTemplate -> GetFunction());		
}

void gamepadRegister(Handle<Function> target) {
    Gamepad::initialise(target);
}

NODE_MODULE(nodeGamepad, gamepadRegister)