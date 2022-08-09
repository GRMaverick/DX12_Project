#ifndef __CLParser_h__
#define __CLParser_h__

#include <vector>

namespace SysUtilities
{
	class CLParser
	{
	public:
		static CLParser* Instance(void);
		~CLParser(void);

		bool			Initialise(char* pCmds);

		bool			HasArgument(const char* _pArgName);
		const char* GetArgument(const char* _pArgName);

	private:
		struct CLArgument
		{
			char pName[32];
			char pValue[32];
		};

		CLParser(void);

		unsigned int	m_NumArgs;
		CLArgument* m_pArguments;
	};
}

#endif __CLParser_h__