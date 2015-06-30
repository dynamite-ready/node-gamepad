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
	NanScope();
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
		Handle<Array> value = NanNew<Array>(currentGamepad -> buttons);
		for(unsigned int i = 0; i < currentGamepad -> buttons; i++)
			value -> Set(i, NanNew<Integer>(SDL_JoystickGetButton(currentGamepad -> object, i)));
		NanReturnValue(value);
	} else NanReturnUndefined();
}

// Get pad/stick input data.
NAN_METHOD(Gamepad::getAxisStateArray){
	NanScope();
	Gamepad* currentGamepad = ObjectWrap::Unwrap<Gamepad>(args.This());
	if(currentGamepad -> object){
		SDL_JoystickUpdate();
		Handle<Array> value = NanNew<Array>(currentGamepad -> axes);
		for(unsigned int i = 0; i < currentGamepad -> axes; i++)
			value -> Set(i, NanNew<Number>(SDL_JoystickGetAxis(currentGamepad -> object, i)));
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
		Handle<Boolean> value = NanNew<Boolean>(connectionStatus == SDL_TRUE ? true : false);
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
	Handle<Integer> value = NanNew<Integer>(currentGamepad -> deviceID);
	NanReturnValue(value);
}

NAN_GETTER(Gamepad::getDeviceName){
	NanScope();
	Gamepad* currentGamepad = ObjectWrap::Unwrap<Gamepad>(args.This());
	if(currentGamepad -> object){
		Handle<String> value = NanNew<String>(currentGamepad -> deviceName);
		NanReturnValue(value);
	} else NanReturnUndefined();
}

NAN_GETTER(Gamepad::getButtonCount){
	NanScope();
	Gamepad* currentGamepad = ObjectWrap::Unwrap<Gamepad>(args.This());
	if(currentGamepad -> object){
		Handle<Integer> value = NanNew<Integer>(currentGamepad -> buttons);
		NanReturnValue(value);
	} else NanReturnUndefined();
}

NAN_GETTER(Gamepad::getAxesCount){
	NanScope();
	Gamepad* currentGamepad = ObjectWrap::Unwrap<Gamepad>(args.This());
	if(currentGamepad -> object){
		Handle<Integer> value = NanNew<Integer>(currentGamepad -> axes);
		NanReturnValue(value);
	} else NanReturnUndefined();
}

NAN_GETTER(Gamepad::getDeviceCount){
	NanScope();
	SDL_JoystickUpdate();
	Handle<Integer> value = NanNew<Integer>(SDL_NumJoysticks());
	NanReturnValue(value);
}

NAN_GETTER(Gamepad::getDeviceIndex){
	NanScope();
	Gamepad* currentGamepad = ObjectWrap::Unwrap<Gamepad>(args.This());
	if(currentGamepad -> object){
		Handle<Integer> value = NanNew<Integer>(currentGamepad -> deviceIndex);
		NanReturnValue(value);
	} else NanReturnUndefined();
}

void Gamepad::initialise(Handle<Object> exports){
	NanScope();
	
	// Start the SDL joystick subsystem.
	SDL_Init(SDL_INIT_JOYSTICK);
	
	Local<String> name = NanNew<String>("Gamepad");
	// Specify and create the Javascript based object constructor.
    Local<FunctionTemplate> constructorTemplate = NanNew<FunctionTemplate>(Gamepad::New);
    constructorTemplate -> InstanceTemplate() -> SetInternalFieldCount(1); // Brr...
    constructorTemplate -> SetClassName(name);
	
	// Create the native object prototype.
	Local<ObjectTemplate> proto = constructorTemplate -> PrototypeTemplate();
	proto -> SetInternalFieldCount(1);
	
	// Prototypal properties.
	proto -> SetAccessor(NanNew<String>("deviceId"), getDeviceId);
	proto -> SetAccessor(NanNew<String>("deviceIndex"), getDeviceIndex);
	proto -> SetAccessor(NanNew<String>("name"), getDeviceName);
	proto -> SetAccessor(NanNew<String>("buttons"), getButtonCount);
	proto -> SetAccessor(NanNew<String>("axes"), getAxesCount);
	
	// Prototype methods...
	proto -> Set(
		NanNew<String>("getButtonStateArray"),
		NanNew<FunctionTemplate>(getButtonStateArray) -> GetFunction()
	);
	
	proto -> Set(
		NanNew<String>("getAxisStateArray"),
		NanNew<FunctionTemplate>(getAxisStateArray) -> GetFunction()
	);

	proto -> Set(
		NanNew<String>("getAttachedStatus"), 
		NanNew<FunctionTemplate>(getAttachedStatus) -> GetFunction()
	);
	
	proto -> Set(
		NanNew<String>("close"), 
		NanNew<FunctionTemplate>(closeDevice) -> GetFunction()
	);	
	
	// To get the initial device count
	// before deciding to create a new object or not.
	exports -> SetAccessor(NanNew<String>("deviceCount"), getDeviceCount);
	// Export the composed persistent container.
	exports -> Set(name, constructorTemplate -> GetFunction());		
}

void gamepadRegister(Handle<Function> target) {
    Gamepad::initialise(target);
}

NODE_MODULE(nodeGamepad, gamepadRegister)