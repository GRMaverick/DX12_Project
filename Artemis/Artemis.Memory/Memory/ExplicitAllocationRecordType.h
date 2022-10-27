#ifndef __ExplicitAllocationRecordType_h__
#define __ExplicitAllocationRecordType_h__

namespace Artemis::Memory
{
	enum class ExplicitAllocationRecordType : unsigned int
	{
		eAllocated,
		eDeallocated,
	};
}
#endif // __ExplicitAllocationRecordType_h__
