module; 

#include <Windows.h>

#include "ArtemisHash.h"

module Artemis.Core:Hashing;

namespace ArtemisCore::Hashing
{
	ArtemisHash SimpleHash(const char* _pBuffer, const size_t& _szLength)
	{
		unsigned long long hash = 5381;

		int c = 0;
		for (unsigned int i = 0; i < _szLength; ++i)
		{
			int c = _pBuffer[i];
			hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
		}

		ArtemisHash hashObj;
		hashObj.Hash = hash;

#if defined(_DEBUG)
		strncpy_s(hashObj.DebugName, _pBuffer, ARRAYSIZE(hashObj.DebugName));
#endif

		return hashObj;
	}
}