#include "includes.h"

void title_thread()
{
	while (!c_console_app_handler::get_ptr()->get_is_opened())
	{
		PROCESS_MEMORY_COUNTERS_EX procmemconuntr{};
		if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&procmemconuntr, sizeof(PROCESS_MEMORY_COUNTERS_EX)))
		{
			char buffer[100];
			sprintf(buffer, "Microsoft Word. RAM Usage: %.1fMb\n", (float)(procmemconuntr.PrivateUsage / 1024.f / 1024.f));
			SetConsoleTitle(buffer);
		}
		Sleep(100);
	}
}

void _startup()
{
	/*time_t now = time(NULL);
	tm* ltm = localtime(&now);

	if (((1 + ltm->tm_mon) != 8) || ltm->tm_year != (2021 - 1900))
		TerminateProcess(GetCurrentProcess(), 0);

	if (ltm->tm_mday < 4 || ltm->tm_mday >= 7)
		TerminateProcess(GetCurrentProcess(), 0);*/

	if (!IsDebuggerPresent())
	{
		auto ultimate_truth = "xui2280.exe";
		auto exe = utilites::executable_name();
		if (std::strcmp(exe, ultimate_truth) != NULL)
		{
			if (std::rename(exe, ultimate_truth) != NULL)
				utilites::shutdown_process();
		}
	}

	utilites::create_thread_fast(title_thread);

	if (c_console_app_handler::get_ptr()->initialize())
		utilites::shutdown_process();
}

auto main() -> int
{
	_startup();

	programm_routine();
}