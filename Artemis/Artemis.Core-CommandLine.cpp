module Artemis.Core:CommandLine;

struct CLArgument
{
	char pName[32];
	char pValue[32];
};

static unsigned int s_NumArgs = 0;
static CLArgument* s_pArguments = nullptr;

namespace ArtemisCore::CommandLine
{
	bool ParseCommandLine(char* _pCmds)
	{
		//LogInfo("Command Line Arguments: %s", pCmds, strlen(pCmds));

		//
		// Process Line
		//
		static const int SEEK_SLASH = 1;
		static const int SEEK_SPACE = 2;
		static const int SEEK_COLON = 3;

		for (int i = 0; i < strlen(_pCmds); ++i)
		{
			if (_pCmds[i] == '/')
			{
				++s_NumArgs;
			}
		}

		s_pArguments = new CLArgument[s_NumArgs];

		int iCurrentArg = -1;
		int iBytesToRead = 0;
		int iState = SEEK_SLASH;
		char* pHead = nullptr;
		for (int i = 0; i < strlen(_pCmds) + 1; ++i)
		{
			switch (iState)
			{
			case SEEK_SLASH:
				if (_pCmds[i] == '/')
				{
					pHead = &_pCmds[i + 1];
					iBytesToRead = 0;
					++iCurrentArg;
					iState = SEEK_COLON;
				}
				break;
			case SEEK_SPACE:
				if (_pCmds[i] == ' ' || _pCmds[i] == '\0')
				{
					iState = SEEK_SLASH;
					strncpy_s(s_pArguments[iCurrentArg].pValue, pHead, iBytesToRead < ARRAYSIZE(s_pArguments[iCurrentArg].pName) ? iBytesToRead - 1 : ARRAYSIZE(s_pArguments[iCurrentArg].pName));
				}
				break;
			case SEEK_COLON:
				if (_pCmds[i] == ':')
				{
					strncpy_s(s_pArguments[iCurrentArg].pName, pHead, iBytesToRead < ARRAYSIZE(s_pArguments[iCurrentArg].pName) ? iBytesToRead - 1 : ARRAYSIZE(s_pArguments[iCurrentArg].pName));
					iBytesToRead = 0;
					pHead = &_pCmds[i + 1];

					iState = SEEK_SPACE;
				}
				else if (_pCmds[i] == ' ' || _pCmds[i] == '\0')
				{
					strncpy_s(s_pArguments[iCurrentArg].pName, pHead, iBytesToRead < ARRAYSIZE(s_pArguments[iCurrentArg].pName) ? iBytesToRead - 1 : ARRAYSIZE(s_pArguments[iCurrentArg].pName));
					strncpy_s(s_pArguments[iCurrentArg].pValue, "1", 1);
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
		//LogInfo("Command Line Arguments Parsed:");
		for (unsigned int i = 0; i < s_NumArgs; ++i)
		{
			//LogInfo("\t%s : %s", m_pArguments[i].pName, m_pArguments[i].pValue);
		}
		return true;
	}

	bool HasArgument(const char* _pArgName)
	{
		for (unsigned int i = 0; i < s_NumArgs; ++i)
		{
			if (strncmp(s_pArguments[i].pName, _pArgName, ARRAYSIZE(s_pArguments[i].pName)) == 0)
				return true;
		}
		return false;
	}

	const char* GetArgument(const char* _pArgName)
	{
		for (unsigned int i = 0; i < s_NumArgs; ++i)
		{
			if (strncmp(s_pArguments[i].pName, _pArgName, ARRAYSIZE(s_pArguments[i].pName)) == 0)
				return s_pArguments[i].pValue;
		}
		return nullptr;
	}
}