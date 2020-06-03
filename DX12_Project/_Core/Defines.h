#pragma once

#include <stdarg.h>
#include <stdio.h>
#include <assert.h>

#define PRAGMA_TODO(todo) __pragma(message("[TODO]: "todo));

#define BACK_BUFFERS 2

inline void DebugPrint(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    //if (debugPrint & level) {
        vfprintf(stdout, format, args);
	//}
    va_end(args);
}

#define VALIDATE_D3D(test) { \
	HRESULT hr = test; \
	if(FAILED(hr)) \
	{ \
		DebugPrint("[HRESULT Failure]: 0x%i", hr); \
		assert(false); \
	} \
} \