#include <Windows.h>

import Artemis.Core;

using namespace ArtemisCore;

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPreviousInstance, LPSTR pCmds, INT nCmdShow)
{
	char pBuffer[] = "Hello World!";
	Hashing::ArtemisHash hash = Hashing::SimpleHash(pBuffer, ARRAYSIZE(pBuffer));

	bool bRes = CommandLine::ParseCommandLine(pCmds);
	bool bRes2 = CommandLine::HasArgument("Width");
	bool bRes3 = CommandLine::HasArgument("Height");
	const char* pArg1 = CommandLine::GetArgument("Width");
	const char* pArg2 = CommandLine::GetArgument("Height");

	Logging::ArtemisLogger::SetCategory(Logging::ArtemisLogger::CATEGORY_APP);
	Logging::ArtemisLogger::SetSeverity(Logging::ArtemisLogger::SEVERITY_INFO);

	Logging::ArtemisLogger::Log(
		Logging::ArtemisLogger::SEVERITY_INFO,
		Logging::ArtemisLogger::CATEGORY_APP,
		"Finished!"
	);
}