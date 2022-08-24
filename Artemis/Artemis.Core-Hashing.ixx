export module Artemis.Core:Hashing;

#include "ArtemisHash.h"

namespace ArtemisCore::Hashing
{
	export struct ArtemisHash;

	export ArtemisHash SimpleHash(const char* _pBuffer, const size_t& _szLength);
}