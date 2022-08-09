#pragma once

#include "D3D12\Device\CommandList.h"
namespace SysUtilities
{
	class RenderMarker
	{
	private:
		CommandList* m_pCmdList = nullptr;
	public:
		template<typename... Args>
		RenderMarker(CommandList* pCmdList, const char* _pFormatString, Args... _args)
		{
			m_pCmdList = pCmdList;
			m_pCmdList->StartMarker(_pFormatString, _args...);
		}

		~RenderMarker()
		{
			m_pCmdList->EndMarker();
		}
	};
}