#pragma once

namespace ArtemisCore::Hashing
{
	struct ArtemisHash
	{
		unsigned long long	Hash;
#if defined(_DEBUG)
		char				DebugName[64];
#endif
	};
}