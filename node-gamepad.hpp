#ifndef node-gamepad_HPP
	#include <v8.h>
	#include <nan.h>
	#include <node.h>
	#include <SDL.h>

	// A super simple class to wrap the struct from the Gamepad library.
	class Gamepad: public node::ObjectWrap {
		public:
			Gamepad(unsigned int deviceNumber);
			~Gamepad();
			
			// Bolier plate.
			static v8::Persistent<v8::FunctionTemplate> constructor;
			static void initialise(v8::Handle<v8::Object> exports); // Main...	
			
			// Javascript constructor.
			static NAN_METHOD(New);
			
			// Class methods
			static NAN_METHOD(getButtonStateArray);
			static NAN_METHOD(getAxisStateArray);
			static NAN_METHOD(getAttachedStatus); // Is the device attached (bool).
			static NAN_METHOD(closeDevice);
			
			// Readonly property getters.
			static NAN_GETTER(getDeviceId);
			static NAN_GETTER(getDeviceName);
			static NAN_GETTER(getButtonCount);
			static NAN_GETTER(getAxesCount);
			static NAN_GETTER(getDeviceCount);
			static NAN_GETTER(getDeviceIndex); // Requested player position;
					
		private:
			SDL_Joystick* object;
			int axes;
			int buttons;
			int deviceID;
			const char* deviceName;
			unsigned int deviceIndex; // The requested 'player' position.
	};
#endif