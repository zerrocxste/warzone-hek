extern BOOL WINAPI CtrlHandler(DWORD CtrlType);

class c_console_app_handler
{
private:
	bool m_on_exit_event;
public:
	bool get_is_opened()
	{
		return m_on_exit_event;
	}

	void shutdown()
	{
		m_on_exit_event = true;
	}

	bool initialize()
	{
		return SetConsoleCtrlHandler(CtrlHandler, TRUE);
	}

	static auto get_ptr()
	{
		static auto ptr = new c_console_app_handler();
		return ptr;
	}
};