#include "../includes.h"

BOOL WINAPI CtrlHandler(DWORD CtrlType)
{
	switch (CtrlType)
	{
	case CTRL_CLOSE_EVENT:
		c_console_app_handler::get_ptr()->shutdown();
		Sleep(1000);
		return TRUE;
	default:
		return FALSE;
	}
	return FALSE;
}