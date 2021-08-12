#include "../includes.h"

const char* utilites::executable_name()
{
	constexpr auto buff_sz = MAX_PATH;
	static char buff[MAX_PATH];
	if (GetModuleFileName(GetModuleHandle(NULL), buff, buff_sz))
	{
		for (INT_PTR i = strlen(buff); i > 0; i--)
		{
			if (buff[i] && buff[i] == '\\')
			{
				strcpy_s(buff, strlen(buff) - i, buff + i + 1);
				break;
			}
		}
	}
	return buff;
}

const char* utilites::rand_string(int length)
{
	constexpr auto _ch_min = 97;
	constexpr auto _ch_max = 122;

	char* result = new char[length];

	for (int i = 0; i < length; i++)
	{
		result[i] = _ch_min + rand() % (_ch_max - _ch_min);
	}

	return result;
}

PROCESSENTRY32* utilites::get_process(const char* exe_name)
{
	auto spanshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (spanshot == INVALID_HANDLE_VALUE)
		return NULL;

	PROCESSENTRY32 pe32{};
	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(spanshot, &pe32))
	{
		CloseHandle(spanshot);
		return NULL;
	}

	do
	{
		if (!strcmp(pe32.szExeFile, exe_name))
		{
			CloseHandle(spanshot);
			return &pe32;
		}
	} while (Process32Next(spanshot, &pe32));

	CloseHandle(spanshot);

	return NULL;
}

MODULEENTRY32* utilites::find_module(DWORD pid, const char* module_name)
{
	auto spanshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);

	if (spanshot == INVALID_HANDLE_VALUE)
	{
		CloseHandle(spanshot);
		return NULL;
	}

	MODULEENTRY32 me32{};

	me32.dwSize = sizeof(MODULEENTRY32);

	if (!Module32First(spanshot, &me32))
	{
		CloseHandle(spanshot);
		return NULL;
	}

	do
	{
		if (!strcmp(me32.szModule, module_name))
		{
			CloseHandle(spanshot);
			return &me32;
		}
		Sleep(1);
	} while (Module32Next(spanshot, &me32));

	CloseHandle(spanshot);

	return NULL;
}

DWORD_PTR utilites::compare_mem(const char* pattern, const char* mask, DWORD_PTR base, DWORD_PTR size, const int patternLength, DWORD speed)
{
	for (DWORD_PTR i = 0; i < size - patternLength; i += speed)
	{
		bool found = true;
		for (DWORD_PTR j = 0; j < patternLength; j++)
		{
			if (mask[j] == '?')
				continue;

			if (pattern[j] != *(char*)(base + i + j))
			{
				found = false;
				break;
			}
		}

		if (found)
		{
			return base + i;
		}
	}

	return NULL;
}

DWORD_PTR utilites::pattern_scanner_ex(
	HANDLE handle,
	DWORD_PTR start, DWORD_PTR end,
	const char* pattern, const char* mask,
	DWORD scan_speed,
	DWORD page_protection, DWORD page_state, DWORD page_type)
{
	DWORD_PTR result = NULL;

	auto pattern_length = strlen(mask);

	MEMORY_BASIC_INFORMATION mbi{};
	while (start < end &&
		VirtualQueryEx(handle, (void*)start, &mbi, sizeof(MEMORY_BASIC_INFORMATION)) != NULL)
	{
		auto fix_seg = false;
		DWORD_PTR start_seg = (DWORD_PTR)mbi.BaseAddress;
		DWORD_PTR size_seg = mbi.RegionSize;

		if (mbi.Protect == page_protection
			&& mbi.State == page_state
			&& mbi.Type == page_type)
		{
			if (start > start_seg)
			{
				size_seg -= start - start_seg;
				fix_seg = true;
			}

			char* seg_buffer = new char[size_seg];

			if (seg_buffer && 
				ReadProcessMemory(handle, (void*)start, seg_buffer, size_seg, NULL))
			{
				if (auto compare_result = compare_mem(pattern, mask, (DWORD_PTR)seg_buffer, size_seg, pattern_length, scan_speed))
				{
					DWORD_PTR offset_from_start = compare_result - (DWORD_PTR)seg_buffer;
					DWORD_PTR seg_buffer_to_internal_addres = start + offset_from_start;
					result = seg_buffer_to_internal_addres;
				}
			}

			delete[] seg_buffer;

			if (result)
				break;
		}
		fix_seg ? start += size_seg : start = start_seg + size_seg;
	}

	return result;
}

//fedor narkoman............
DWORD_PTR utilites::asm64_solve_dest(DWORD64 src, DWORD rva)
{
	auto dest = src + rva;
	if (rva > 0x7FFFFFFF)
	{
		auto rel64 = (DWORD)src + (DWORD64)rva;
		auto rel32 = (DWORD)dest;
		dest -= rel64 - rel32;
	}
	return dest;
}

void utilites::shutdown_process(HANDLE handle)
{
	TerminateProcess(handle, EXIT_SUCCESS);
}

bool utilites::create_thread_fast(void* function, void* arg)
{
	auto th_handle = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)function, arg, NULL, NULL);

	if (!th_handle)
		return false;

	CloseHandle(th_handle);

	return true;
}

bool utilites::print_bytes_ex(HANDLE handle, DWORD_PTR va, size_t length)
{
	BYTE* bytes = new BYTE[length];

	if (!bytes)
		return false;

	auto ret = true;

	printf("\n[+] %s, virtual address = %p\n", __FUNCTION__, va);
	for (INT_PTR i = 0; i < length; i++)
	{
		if (!ReadProcessMemory(handle, (void*)(va + i), (void*)(bytes + i), sizeof(BYTE), NULL))
		{
			printf("\n[-] %s, failed. ReadProcessMemory error code = %X", __FUNCTION__, GetLastError());
			ret = false;
			break;
		}
		printf("%02X ", bytes[i]);
	}
	printf("\n\n");

	delete[] bytes;

	return ret;
}