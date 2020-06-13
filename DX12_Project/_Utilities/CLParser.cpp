#include "Defines.h"
#include "CLParser.h"

CLParser::CLParser(void)
{
	m_NumArgs = 0;
	m_pArguments = nullptr;
}

CLParser::~CLParser(void)
{
	if (m_pArguments) delete[] m_pArguments;
}

CLParser* CLParser::Instance(void)
{
	static CLParser rParser;
	return &rParser;
}

bool CLParser::Initialise(LPSTR pCmds)
{
	LogInfo_App("Command Line Arguments: %s", pCmds, strlen(pCmds));

	//
	// Process Line
	//
	static const int SEEK_SLASH = 1;
	static const int SEEK_SPACE = 2;
	static const int SEEK_COLON = 3;

	for (int i = 0; i < strlen(pCmds); ++i)
	{
		if (pCmds[i] == '/') ++m_NumArgs;
	}

	m_pArguments = new CLArgument[m_NumArgs];

	int iCurrentArg = -1;
	int iBytesToRead = 0;
	int iState = SEEK_SLASH;
	char* pHead = nullptr;
	for (int i = 0; i < strlen(pCmds)+1; ++i)
	{
		switch (iState)
		{
		case SEEK_SLASH:
			if (pCmds[i] == '/')
			{
				pHead = &pCmds[i+1];
				iBytesToRead = 0;
				++iCurrentArg;
				iState = SEEK_COLON;
			}
			break;
		case SEEK_SPACE:			
			if (pCmds[i] == ' ' || pCmds[i] == '\0')
			{
				iState = SEEK_SLASH;
				strncpy_s(m_pArguments[iCurrentArg].pValue, pHead, iBytesToRead < ARRAYSIZE(m_pArguments[iCurrentArg].pName) ? iBytesToRead-1 : ARRAYSIZE(m_pArguments[iCurrentArg].pName));
			}
			break;
		case SEEK_COLON:
			if (pCmds[i] == ':')
			{
				strncpy_s(m_pArguments[iCurrentArg].pName, pHead, iBytesToRead < ARRAYSIZE(m_pArguments[iCurrentArg].pName) ? iBytesToRead-1 : ARRAYSIZE(m_pArguments[iCurrentArg].pName));
				iBytesToRead = 0;
				pHead = &pCmds[i + 1];

				iState = SEEK_SPACE;
			}
			else if (pCmds[i] == ' ' || pCmds[i] == '\0')
			{
				strncpy_s(m_pArguments[iCurrentArg].pName, pHead, iBytesToRead < ARRAYSIZE(m_pArguments[iCurrentArg].pName) ? iBytesToRead-1 : ARRAYSIZE(m_pArguments[iCurrentArg].pName));
				strncpy_s(m_pArguments[iCurrentArg].pValue, "1", 1);
				iState = SEEK_SLASH;
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
	LogInfo_App("Command Line Arguments Parsed:");
	for (UINT i = 0; i < m_NumArgs; ++i)
	{
		LogInfo_App("\t%s : %s", m_pArguments[i].pName, m_pArguments[i].pValue);
	}
	return true;
}

bool CLParser::HasArgument(const char* _pArgName)
{
	for (UINT i = 0; i < m_NumArgs; ++i)
	{
		if (strncmp(m_pArguments[i].pName, _pArgName, ARRAYSIZE(m_pArguments[i].pName)) == 0)
			return true;
	}
	return false;
}

const char* CLParser::GetArgument(const char* _pArgName)
{
	for (UINT i = 0; i < m_NumArgs; ++i)
	{
		if(strncmp(m_pArguments[i].pName, _pArgName, ARRAYSIZE(m_pArguments[i].pName)) == 0)
			return m_pArguments[i].pValue;
	}
	return nullptr;
}