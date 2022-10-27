#pragma once

#include "Interfaces/ICommandList.h"

namespace Artemis::Renderer::Helpers
{
	class RenderMarker
	{
	private:
		Renderer::Interfaces::ICommandList* m_pCmdList = nullptr;

	public:
		RenderMarker( Renderer::Interfaces::ICommandList* pCmdList, const char* _pFormatString )
		{
			m_pCmdList = pCmdList;
			m_pCmdList->StartMarker( _pFormatString );
		}

		~RenderMarker()
		{
			m_pCmdList->EndMarker();
		}
	};
}
