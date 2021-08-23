#include "includes.h"

void title_thread()
{
	auto h_current_process = GetCurrentProcess();
	while (!console_app_handler::m_on_exit_event)
	{
		PROCESS_MEMORY_COUNTERS_EX pmcex{};
		if (GetProcessMemoryInfo(h_current_process, (PROCESS_MEMORY_COUNTERS*)&pmcex, sizeof(PROCESS_MEMORY_COUNTERS_EX)))
		{
			char buffer[100];
			sprintf(buffer, "Microsoft Word. RAM Usage: %.1fMb\n", (float)(pmcex.PrivateUsage / 1024.f / 1024.f));
			SetConsoleTitle(buffer);
		}
		Sleep(100);
	}
}

bool startup()
{
#ifndef CONSOLE
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif

	if (!console_app_handler::initialize())
		return false;

#ifdef CONSOLE
	if (!utilites::create_thread_fast(title_thread))
		return false;
#endif

	/*time_t now = time(NULL);
	tm* ltm = localtime(&now);

	if (((1 + ltm->tm_mon) != 8) || ltm->tm_year != (2021 - 1900))
		return false;

	if (ltm->tm_mday < 6 || ltm->tm_mday >= 9)
		return false;*/

	return true;
}

int main()
{
	if (!startup())
		utilites::shutdown_process();

	hack::pornhub_invoke();
}