#ifndef __MemoryTracking_h__
#define __MemoryTracking_h__

#include <vector>
#include <cstdlib>

#include "MemoryContextData.h"
#include "MemoryContextCategory.h"
#include "ExplicitAllocationRecordType.h"

namespace Artemis::Memory
{
	class MemoryGlobalTracking
	{
	public:
		static void*                 Allocate( std::size_t _size );
		static void                  Deallocate( void* _pAddress );
		static void                  SetContext( MemoryContextCategory _eCategory );
		static const char*           GetContextName( MemoryContextCategory _eCategory );
		static MemoryContextData     GetContextStats( MemoryContextCategory _eCategory );
		static MemoryContextCategory GetCurrentContext( void );

		static void RecordExplicitAllocation( MemoryContextCategory _eCategory, void* _pAddress, std::size_t _szAllocation );
		static void RecordExplicitDellocation( const void* _pAddress );
	private:
		struct ExplicitRecord
		{
			void*                 Address;
			std::size_t           Size;
			MemoryContextCategory Category;
		};

		typedef std::vector<ExplicitRecord> ExplicitRecords;

		static MemoryContextCategory m_eCategory;
		static ExplicitRecords       m_vExplicitRecords;
	};
}

#endif // __MemoryTracking_h__
