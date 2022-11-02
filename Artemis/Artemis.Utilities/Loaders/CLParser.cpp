#include "Defines.h"
#include "CLParser.h"

#include <cstring>

namespace Artemis::Utilities
{
	CLParser::CLParser( void )
	{
		m_uiNumArgs  = 0;
		m_pArguments = nullptr;
	}

	CLParser::~CLParser( void )
	{
		if ( m_pArguments )
			delete[] m_pArguments;
	}

	CLParser* CLParser::Instance( void )
	{
		static CLParser rParser;
		return &rParser;
	}

	bool CLParser::Initialise( char* _pCmds )
	{
		LogInfo( "Command Line Arguments: %s", _pCmds, strlen(_pCmds) );

		//
		// Process Line
		//
		static constexpr int kSeekSlash = 1;
		for ( int i = 0; i < strlen( _pCmds ); ++i )
		{
			if ( _pCmds[i] == '/' )
				++m_uiNumArgs;
		}

		m_pArguments = new CLArgument[m_uiNumArgs];

		int         iCurrentArg  = -1;
		int         iBytesToRead = 0;
		int         iState       = kSeekSlash;
		const char* pHead        = nullptr;
		for ( int i = 0; i < strlen( _pCmds ) + 1; ++i )
		{
			static constexpr int kSeekSpace = 2;
			static constexpr int kSeekColon = 3;

			switch ( iState )
			{
				case kSeekSlash:
					if ( _pCmds[i] == '/' )
					{
						pHead        = &_pCmds[i + 1];
						iBytesToRead = 0;
						++iCurrentArg;
						iState = kSeekColon;
					}
					break;
				case kSeekSpace:
					if ( _pCmds[i] == ' ' || _pCmds[i] == '\0' )
					{
						iState = kSeekSlash;
						strncpy_s( m_pArguments[iCurrentArg].pValue, pHead, iBytesToRead < ARRAYSIZE( m_pArguments[iCurrentArg].pName ) ? iBytesToRead - 1 : ARRAYSIZE( m_pArguments[iCurrentArg].pName ) );
					}
					break;
				case kSeekColon:
					if ( _pCmds[i] == ':' )
					{
						strncpy_s( m_pArguments[iCurrentArg].pName, pHead, iBytesToRead < ARRAYSIZE( m_pArguments[iCurrentArg].pName ) ? iBytesToRead - 1 : ARRAYSIZE( m_pArguments[iCurrentArg].pName ) );
						iBytesToRead = 0;
						pHead        = &_pCmds[i + 1];

						iState = kSeekSpace;
					}
					else if ( _pCmds[i] == ' ' || _pCmds[i] == '\0' )
					{
						strncpy_s( m_pArguments[iCurrentArg].pName, pHead, iBytesToRead < ARRAYSIZE( m_pArguments[iCurrentArg].pName ) ? iBytesToRead - 1 : ARRAYSIZE( m_pArguments[iCurrentArg].pName ) );
						strncpy_s( m_pArguments[iCurrentArg].pValue, "1", 1 );
						iState = kSeekSlash;
					}
					break;
				default:
					return false;
			}
			++iBytesToRead;
		}

		//
		// Log Arguments
		//
		LogInfo( "Command Line Arguments Parsed:" );
		for ( unsigned int i = 0; i < m_uiNumArgs; ++i )
		{
			LogInfo( "\t%s : %s", m_pArguments[i].pName, m_pArguments[i].pValue );
		}
		return true;
	}

	bool CLParser::HasArgument( const char* _pArgName ) const
	{
		for ( unsigned int i = 0; i < m_uiNumArgs; ++i )
		{
			if ( strncmp( m_pArguments[i].pName, _pArgName, ARRAYSIZE( m_pArguments[i].pName ) ) == 0 )
				return true;
		}
		return false;
	}

	const char* CLParser::GetArgument( const char* _pArgName ) const
	{
		for ( unsigned int i = 0; i < m_uiNumArgs; ++i )
		{
			if ( strncmp( m_pArguments[i].pName, _pArgName, ARRAYSIZE( m_pArguments[i].pName ) ) == 0 )
				return m_pArguments[i].pValue;
		}
		return nullptr;
	}
}
