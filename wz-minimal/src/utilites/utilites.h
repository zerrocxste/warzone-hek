namespace utilites
{
	const char* executable_name();
	const char* rand_string(int length);
	PROCESSENTRY32* get_process(const char* exe_name);
	MODULEENTRY32* find_module(DWORD pid, const char* module_name);
	DWORD_PTR compare_mem(const char* pattern, const char* mask, DWORD_PTR base, DWORD_PTR size, const int patternLength, DWORD speed);
	DWORD_PTR pattern_scanner_ex(
		HANDLE handle,
		DWORD_PTR start, DWORD_PTR end,
		const char* pattern, const char* mask,
		DWORD scan_speed, 
		DWORD page_protection = PAGE_EXECUTE_READ, DWORD page_state = MEM_COMMIT, DWORD page_type = MEM_PRIVATE);
	DWORD_PTR asm64_solve_dest(DWORD64 src, DWORD rva);
	void shutdown_process(HANDLE handle = GetCurrentProcess());
	bool create_thread_fast(void* function, void* arg = NULL);
	bool print_bytes_ex(HANDLE handle, DWORD_PTR va, size_t length);
}