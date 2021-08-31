#include "includes.h"

#ifdef DEBUG
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
#endif

bool startup()
{
	if (!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS))
		return false;

#ifdef ON_TOP
	SetWindowPos(GetConsoleWindow(), HWND_TOPMOST, 0, 0, 0, 0, SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	ShowWindow(GetConsoleWindow(), SW_NORMAL);
#endif

#ifndef SHOW_CONSOLE
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif

	if (!console_app_handler::initialize())
		return false;

#ifdef DEBUG
	if (!utilites::create_thread_fast(title_thread))
		return false;
#endif

	return true;
}

int main()
{
	if (!startup())
		utilites::shutdown_process();

	hack::pornhub_invoke();
}