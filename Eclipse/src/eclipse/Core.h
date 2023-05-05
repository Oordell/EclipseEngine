#pragma once

#ifdef EC_PLATFORM_WINDOWS
	#ifdef EC_BUILD_DLL
		#define ECLIPSE_API _declspec(dllexport)
	#else
		#define ECLIPSE_API _declspec(dllexport)
	#endif
#else
	#error Eclipse only supports Windows!
#endif