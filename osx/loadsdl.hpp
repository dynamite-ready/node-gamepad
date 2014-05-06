#ifndef loadsdl_HPP
	#include <dlfcn.h>
	void* loadLibrary = dlopen("SDL2", RTLD_LAZY);
#endif