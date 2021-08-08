#include "../includes.h"

BOOL WINAPI CtrlHandler(DWORD CtrlType)
{
	switch (CtrlType)
	{
	case CTRL_CLOSE_EVENT:
		console_app_handler::m_on_exit_event = true;
		Sleep(1000);
		return TRUE;
	default:
		return FALSE;
	}
	return FALSE;
}

namespace console_app_handler
{
	bool m_on_exit_event = false;
	bool initialize()
	{
		return SetConsoleCtrlHandler(CtrlHandler, TRUE);
	}
}