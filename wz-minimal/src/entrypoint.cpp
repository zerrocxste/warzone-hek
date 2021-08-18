#include "includes.h"

void title_thread()
{
	while (!console_app_handler::m_on_exit_event)
	{
		PROCESS_MEMORY_COUNTERS_EX pmce{};
		if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmce, sizeof(PROCESS_MEMORY_COUNTERS_EX)))
		{
			char buffer[100];
			sprintf(buffer, "Microsoft Word. RAM Usage: %.1fMb\n", (float)(pmce.PrivateUsage / 1024.f / 1024.f));
			SetConsoleTitle(buffer);
		}
		Sleep(100);
	}
}

bool startup()
{
	if (!console_app_handler::initialize())
		return false;

	if (!utilites::create_thread_fast(title_thread))
		return false;

	/*time_t now = time(NULL);
	tm* ltm = localtime(&now);

	if (((1 + ltm->tm_mon) != 8) || ltm->tm_year != (2021 - 1900))
		return false;

	if (ltm->tm_mday < 6 || ltm->tm_mday >= 9)
		return false;*/

	if (!IsDebuggerPresent())
	{
		auto ultimate_truth = "xui2280.exe";
		auto exe = utilites::executable_name();
		if (std::strcmp(exe, ultimate_truth) != 0
			&& std::rename(exe, ultimate_truth) != 0)
		{
			printf("[+] u are welcome to alternativehack.gay\n");
			return false;
		}
	}

	return true;
}

int main()
{
	if (!startup())
		utilites::shutdown_process();

	hack::pornhub_invoke();
}