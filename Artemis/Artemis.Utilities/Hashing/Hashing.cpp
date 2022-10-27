#include "Hashing.h"

namespace Artemis::Utilities
{
	unsigned long SimpleHash( const char* _pBuffer, const size_t& _szLength )
	{
		unsigned long hash = 5381;

		int c = 0;
		for ( unsigned int i = 0; i < _szLength; ++i )
		{
			int c = _pBuffer[i];
			hash  = ((hash << 5) + hash) + c; /* hash * 33 + c */
		}

		return hash;
	}
}
