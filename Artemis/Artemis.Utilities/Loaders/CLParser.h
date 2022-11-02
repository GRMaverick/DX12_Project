#ifndef __CLParser_h__
#define __CLParser_h__

namespace Artemis::Utilities
{
	class CLParser
	{
	public:
		static CLParser* Instance( void );
		~CLParser( void );

		bool Initialise( char* _pCmds );

		bool        HasArgument( const char* _pArgName ) const;
		const char* GetArgument( const char* _pArgName ) const;

	private:
		struct CLArgument
		{
			char pName[32];
			char pValue[32];
		};

		CLParser( void );

		unsigned int m_uiNumArgs;
		CLArgument*  m_pArguments;
	};
}

#endif __CLParser_h__
