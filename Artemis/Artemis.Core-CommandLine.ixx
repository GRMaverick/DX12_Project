export module Artemis.Core:CommandLine;

import <stdio.h>;
import <string.h>;
import <Windows.h>;

namespace ArtemisCore::CommandLine
{
	export bool ParseCommandLine(char* _pCmds);
	export bool HasArgument(const char* _pArgName);
	export const char* GetArgument(const char* _pArgName);
}