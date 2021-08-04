﻿#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <psapi.h>

struct
{
	HANDLE access_handle;
	DWORD pid;
	DWORD_PTR base_address;
	DWORD_PTR base_size;
	DWORD_PTR base_end;
	DWORD_PTR min_application_address_space;
	DWORD_PTR max_application_address_space;
} mw_process;

struct
{
	DWORD_PTR prologue_encrypted_function;
	DWORD_PTR game_state_struct;
	DWORD_PTR weapon_recoil_x_axis;
	DWORD_PTR weapon_recoil_y_axis;
	DWORD_PTR weapon_breath_x_axis;
	DWORD_PTR weapon_breath_y_axis;
	//DWORD_PTR weapon_spread; //spread serverside imho
	//DWORD_PTR weapon_spread_crosshair_shake;
	DWORD_PTR radar_draw_enemy;
	//DWORD_PTR radar_draw_enemy_angles; // this shit activates MBPLA, no za kakim to huem ne pokazivaet sam ugol
} offsets;

struct 
{
	BYTE* o_weapon_recoil_x_axis;
	BYTE* o_weapon_recoil_y_axis;
	BYTE* o_weapon_breath_x_axis;
	BYTE* o_weapon_breath_y_axis;
	//BYTE* o_weapon_spread;
	//BYTE* o_weapon_spread_crosshair_shake;
	BYTE* o_radar_draw_enemy;
	//BYTE* o_radar_draw_enemy_angles;
} saved_original_bytes;

struct
{
	BYTE* m_weapon_recoil_x_axis;
	BYTE* m_weapon_recoil_y_axis;
	BYTE* m_weapon_breath_x_axis;
	BYTE* m_weapon_breath_y_axis;
	//BYTE* m_weapon_spread;
	//BYTE* m_weapon_spread_crosshair_shake;
	BYTE* m_radar_draw_enemy;
	//BYTE* m_radar_draw_enemy_angles;
} game_patch_patterns;

bool on_exit_event = false;

DWORD sleep_timer = 0;
bool is_enabled = false;
bool is_key_pressed = false;

const char* executable_name()
{
	constexpr auto buff_sz = MAX_PATH;
	static char buff[MAX_PATH];
	if (GetModuleFileName(GetModuleHandle(NULL), buff, buff_sz))
	{
		for (auto i = strlen(buff); i > 0; i--)
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

const char* rand_string(int length)
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

PROCESSENTRY32* get_process(const char* exe_name)
{
	auto spanshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (spanshot == INVALID_HANDLE_VALUE)
		return NULL;

	PROCESSENTRY32 pe32;
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

MODULEENTRY32* find_module(DWORD pid, const char* module_name)
{
	auto spanshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);

	if (spanshot == INVALID_HANDLE_VALUE)
	{
		CloseHandle(spanshot);
		return NULL;
	}

	MODULEENTRY32 me32;

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

DWORD_PTR compare_mem(const char* pattern, const char* mask, DWORD_PTR base, DWORD_PTR size, const int patternLength, DWORD speed)
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

DWORD_PTR pattern_scanner_ex(
	HANDLE handle, 
	DWORD_PTR start, DWORD_PTR end, 
	const char* pattern, const char* mask, 
	DWORD scan_speed,
	DWORD page_prot = PAGE_EXECUTE_READ, DWORD page_state = MEM_COMMIT, DWORD page_type = MEM_PRIVATE)
{
	auto pattern_length = strlen(mask);

	MEMORY_BASIC_INFORMATION mbi{};
	while (start < end && 
		VirtualQueryEx(handle, (void*)start, &mbi, sizeof(MEMORY_BASIC_INFORMATION)) != NULL)
	{
		auto fix_seg = false;
		DWORD_PTR start_seg = (DWORD_PTR)mbi.BaseAddress;
		DWORD_PTR size_seg = mbi.RegionSize;

		if (mbi.Protect == page_prot
			&& mbi.State == page_state
			&& mbi.Type == page_type)
		{
			if (start > start_seg)
			{
				size_seg -= start - start_seg;
				fix_seg = true;
			}

			char* seg_buffer = new char[size_seg];

			if (ReadProcessMemory(handle, (void*)start, seg_buffer, size_seg, NULL))
			{
				if (auto compare_result = compare_mem(pattern, mask, (DWORD_PTR)seg_buffer, size_seg, pattern_length, scan_speed))
				{
					DWORD_PTR offset_from_start = compare_result - (DWORD_PTR)seg_buffer;
					DWORD_PTR seg_buffer_to_internal_addres = start + offset_from_start;
					delete[] seg_buffer;
					return seg_buffer_to_internal_addres;
				}
			}

			delete[] seg_buffer;
		}
		fix_seg ? start += size_seg : start = start_seg + size_seg;
	}

	return NULL;
}

//fedor narkoman............
DWORD_PTR asm64_solve_dest(DWORD64 src, DWORD relative_address)
{
	auto dest = src + relative_address;
	auto rel64 = (DWORD)src + (DWORD64)relative_address;
	auto rel32 = (DWORD)dest;
	return dest - (rel64 - rel32);
}

BOOL WINAPI CtrlHandler(DWORD CtrlType)
{
	switch (CtrlType)
	{
	case CTRL_CLOSE_EVENT:
		on_exit_event = true;
		Sleep(1000);
		return TRUE;
	default:
		return FALSE;
	}
}

void title_thread()
{
	while (!on_exit_event)
	{
		PROCESS_MEMORY_COUNTERS_EX procmemconuntr{};
		GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&procmemconuntr, sizeof(PROCESS_MEMORY_COUNTERS_EX));
		char buffer[100];
		sprintf(buffer, "Microsoft Word. RAM Usage: %.1fMb\n", (float)(procmemconuntr.PrivateUsage / 1024.f / 1024.f));
		SetConsoleTitle(buffer);
		Sleep(100);
	}
}

void predefinition_game_process()
{
	auto process_name = "ModernWarfare.exe";

	auto process = get_process(process_name);

	if (!process)
	{
		printf("[-] Not found MW19 process\n");
		system("pause");
		exit(1);
	}

	mw_process.pid = process->th32ProcessID;

	auto base = find_module(mw_process.pid, process_name);

	if (!base)
	{
		printf("[-] Not found MW19 module\n");
		system("pause");
		exit(1);
	}

	mw_process.base_address = (DWORD_PTR)base->modBaseAddr;
	mw_process.base_size = (DWORD_PTR)base->modBaseSize;
	mw_process.base_end = mw_process.base_address + mw_process.base_size;

	printf("[+] Found MW19 process. pid = %d, base = 0x%p, base size = %I64X, base end = 0x%p\n",
		mw_process.pid,
		mw_process.base_address,
		mw_process.base_size,
		mw_process.base_end);

	mw_process.access_handle = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION, FALSE, mw_process.pid);

	if (!mw_process.access_handle)
	{
		printf("[-] Failed open process\n");
		system("pause");
		exit(1);
	}

	SYSTEM_INFO info{};
	GetSystemInfo(&info);

	mw_process.min_application_address_space = (DWORD_PTR)info.lpMinimumApplicationAddress;
	mw_process.max_application_address_space = (DWORD_PTR)info.lpMaximumApplicationAddress;
}

void save_original_bytes()
{
	saved_original_bytes.o_weapon_recoil_x_axis = new BYTE[7];
	saved_original_bytes.o_weapon_recoil_y_axis = new BYTE[7];
	saved_original_bytes.o_weapon_breath_x_axis = new BYTE[5];
	saved_original_bytes.o_weapon_breath_y_axis = new BYTE[6];
	//saved_original_bytes.o_weapon_spread = new BYTE[17];
	//saved_original_bytes.o_weapon_spread_crosshair_shake = new BYTE[8];
	saved_original_bytes.o_radar_draw_enemy = new BYTE[2];
	//saved_original_bytes.o_radar_draw_enemy_angles = new BYTE[2];

	ReadProcessMemory(mw_process.access_handle, (void*)offsets.weapon_recoil_x_axis, saved_original_bytes.o_weapon_recoil_x_axis, 7, NULL) ?
		printf("[+] Recoil x axis instruction dumped\n") : printf("[-] Recoil x axis instruction dump failed\n");
		
	ReadProcessMemory(mw_process.access_handle, (void*)offsets.weapon_recoil_y_axis, saved_original_bytes.o_weapon_recoil_y_axis, 7, NULL) ?
		printf("[+] Recoil y axis instruction dumped\n") : printf("[-] Recoil y axis instruction dump failed\n");

	ReadProcessMemory(mw_process.access_handle, (void*)offsets.weapon_breath_x_axis, saved_original_bytes.o_weapon_breath_x_axis, 5, NULL) ?
		printf("[+] Breath x axis instruction dumped\n") : printf("[-] Breath x axis instruction dump failed\n");

	ReadProcessMemory(mw_process.access_handle, (void*)offsets.weapon_breath_y_axis, saved_original_bytes.o_weapon_breath_y_axis, 6, NULL) ?
		printf("[+] Breath y axis instruction dumped\n") : printf("[-] Breath y axis instruction dump failed\n");

	/*ReadProcessMemory(mw_process.handle, (void*)offsets.weapon_spread, saved_original_bytes.o_weapon_spread, 17, NULL) ?
		printf("[+] Weapon spread instruction dumped\n") : printf("[-] Weapon spread instruction dump failed\n");*/

	/*ReadProcessMemory(mw_process.handle, (void*)offsets.weapon_spread_crosshair_shake, saved_original_bytes.o_weapon_spread_crosshair_shake, 8, NULL) ?
		printf("[+] Weapon spread crosshair shake instruction dumped\n") : printf("[+] Weapon spread crosshair shake instruction dump failed\n");*/

	ReadProcessMemory(mw_process.access_handle, (void*)offsets.radar_draw_enemy, saved_original_bytes.o_radar_draw_enemy, 2, NULL) ?
		printf("[+] Radar enemy check instruction dumped\n") : printf("[-] Radar enemy check instruction dump failed\n");

	/*ReadProcessMemory(mw_process.handle, (void*)offsets.radar_draw_enemy_angles, saved_original_bytes.o_radar_draw_enemy_angles, 2, NULL) ?
		printf("[+] Radar enemy angles check instruction dumped\n") : printf("[-] Radar enemy angles check instruction dump failed\n");*/
}

void make_patches()
{
	game_patch_patterns.m_weapon_recoil_x_axis = new BYTE[7];
	game_patch_patterns.m_weapon_recoil_y_axis = new BYTE[7];
	game_patch_patterns.m_weapon_breath_x_axis = new BYTE[5];
	game_patch_patterns.m_weapon_breath_y_axis = new BYTE[6];
	//game_patch_patterns.m_weapon_spread = new BYTE[17];
	//game_patch_patterns.m_weapon_spread_crosshair_shake = new BYTE[8];
	game_patch_patterns.m_radar_draw_enemy = new BYTE[2];
	//game_patch_patterns.m_radar_draw_enemy_angles = new BYTE[2];

	memset(game_patch_patterns.m_weapon_recoil_x_axis, 0x90, 7);
	memset(game_patch_patterns.m_weapon_recoil_y_axis, 0x90, 7);
	memset(game_patch_patterns.m_weapon_breath_x_axis, 0x90, 5);
	memset(game_patch_patterns.m_weapon_breath_y_axis, 0x90, 6);
	/*memcpy(game_patch_patterns.m_weapon_spread, "\xC7\x83\x8C\x07\x00\x00\x00\x00\x16\xC3\x90\x90\x90\x90\x90\x90\x90", 17);
	memcpy((void*)(game_patch_patterns.m_weapon_spread + 0x2), (void*)(saved_original_bytes.o_weapon_spread + 0xD), 4);
	float spread_value = -150.f;
	memcpy((void*)(game_patch_patterns.m_weapon_spread + 0x6), &spread_value, sizeof(float));*/
	//memset(game_patch_patterns.m_weapon_spread_crosshair_shake, 0x90, 8);
	memset(game_patch_patterns.m_radar_draw_enemy, 0x90, 2);
	//memset(game_patch_patterns.m_radar_draw_enemy_angles, 0x90, 2);
}

void find_encrypted_function()
{
	auto e8xxxx_encrypted_func = pattern_scanner_ex(mw_process.access_handle, mw_process.base_address, mw_process.base_end,
		"\xE8\x00\x00\x00\x00\x48\x8D\x00\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x44\x8B\x00\x00\x00\x00\x00\x48\x8D\x00\x00\x00\x00\x00\x4C\x8D", "x????xx?????x????xx?????xx?????xx",
		0x1, PAGE_EXECUTE_READWRITE);

	if (!e8xxxx_encrypted_func)
	{
		printf("[-] Not found required instruction\n");
		system("pause");
		exit(1);
	}

	printf("[+] Found encrypted function caller = 0x%p\n", e8xxxx_encrypted_func);

	DWORD relative_address_encrypted_function = 0;
	ReadProcessMemory(mw_process.access_handle, (void*)(e8xxxx_encrypted_func + 0x1), &relative_address_encrypted_function, sizeof(DWORD), NULL);

	printf("[+] Relative offset to encrypted function from E8 XXXX = 0x%X\n", relative_address_encrypted_function);

	offsets.prologue_encrypted_function = asm64_solve_dest(e8xxxx_encrypted_func + 0x5, relative_address_encrypted_function);

	printf("[+] Encrypted function found = 0x%p\n", offsets.prologue_encrypted_function);
}

void find_game_state_structure()
{
	auto func_7FF73E4C1A40 = pattern_scanner_ex(mw_process.access_handle, mw_process.base_address, mw_process.base_end,
		"\x48\x89\x00\x00\x00\x57\x48\x83\xEC\x00\x48\x8B\x00\x4C\x8D\x00\x00\x00\x00\x00\x48\x8B\x00\xBA\x00\x00\x00\x00\x48\x8B\x00\xE8\x00\x00\x00\x00\x4C\x8D",
		"xx???xxxx?xx?xx?????xx?x????xx?x????xx",
		0x1, PAGE_EXECUTE_READWRITE);

	if (!func_7FF73E4C1A40)
	{
		//"48 8B ? ? ? ? ? BA ? ? ? ? 48 8B ? E8 ? ? ? ? 48 8B ? 48 85 ? 75 ? 33 DB EB ? 0F B7 ? B9 ? ? ? ? 66 3B ? 73"
		if (!(func_7FF73E4C1A40 = pattern_scanner_ex(mw_process.access_handle,
			mw_process.base_address, mw_process.base_end,
			"\x48\x8B\x00\x00\x00\x00\x00\xBA\x00\x00\x00\x00\x48\x8B\x00\xE8\x00\x00\x00\x00\x48\x8B\x00\x48\x85\x00\x75\x00\x33\xDB\xEB\x00\x0F\xB7\x00\xB9\x00\x00\x00\x00\x66\x3B\x00\x73",
			"xx?????x????xx?x????xx?xx?x?xxx?xx?x????xx?x",
			0x1, 
			PAGE_EXECUTE_READWRITE)))
		{
			printf("[-] Not found required function\n");
			system("pause");
			exit(1);
		}
	}
	else
		func_7FF73E4C1A40 += 0x48;

	DWORD relative_address = 0;
	ReadProcessMemory(mw_process.access_handle, (void*)(func_7FF73E4C1A40 + 0x3), &relative_address, sizeof(DWORD), NULL);

	ReadProcessMemory(mw_process.access_handle, (void*)asm64_solve_dest(func_7FF73E4C1A40 + 0x7, relative_address), &offsets.game_state_struct, sizeof(DWORD_PTR), NULL);

	if (!offsets.game_state_struct)
	{
		printf("[-] Not found game state struct\n");
		system("pause");
		exit(1);
	}

	printf("[+] Game state struct = 0x%p\n", offsets.game_state_struct);
}

void find_ofs_for_hack_features()
{
	if (offsets.prologue_encrypted_function && 
		(offsets.weapon_recoil_x_axis = pattern_scanner_ex(mw_process.access_handle,
		offsets.prologue_encrypted_function, 
		mw_process.base_end,
		"\x8B\x06\x41\x89\x80", "xxxxx",
		0x1,
		PAGE_EXECUTE_READWRITE)))
	{
		offsets.weapon_recoil_x_axis += 0x2;

		offsets.weapon_recoil_y_axis = pattern_scanner_ex(mw_process.access_handle,
			offsets.weapon_recoil_x_axis + 0x1,
			offsets.weapon_recoil_x_axis + 0x50,
			"\x41\x89\x80", "xxx",
			0x1,
			PAGE_EXECUTE_READWRITE);
	}

	offsets.weapon_breath_x_axis = pattern_scanner_ex(mw_process.access_handle,
		mw_process.base_address,
		mw_process.base_end,
		"\xF3\x44\x00\x00\x00\xF3\x44\x00\x00\x00\x00\xF3\x44\x00\x00\x00\x00\x48\x8B\x00\x00\x00\x00\x00\x48\x33", "xx???xx????xx????xx?????xx",
		0x1,
		PAGE_EXECUTE_READWRITE);

	offsets.weapon_breath_y_axis = pattern_scanner_ex(mw_process.access_handle,
		mw_process.base_address,
		mw_process.base_end,
		"\xF3\x44\x00\x00\x00\x00\xF3\x44\x00\x00\x00\x00\x48\x8B\x00\x00\x00\x00\x00\x48\x33", "xx????xx????xx?????xx",
		0x1,
		PAGE_EXECUTE_READWRITE);

	//structure offset to += 0x13
	/*if (!(offsets.weapon_spread = pattern_scanner_ex(mw_process.handle,
		mw_process.base_address,
		mw_process.base_end,
		"\x44\x0F\x00\x00\x00\x00\x00\x00\x00\xF3\x0F\x00\x00\x00\x00\x00\x00\x49\x8B", "xx???????xx??????xx",
		0x1,
		PAGE_EXECUTE_READWRITE)))
	{
		printf("[-] First spread pattern is no longer valid, research for second pattern\n");

		if (offsets.weapon_spread = pattern_scanner_ex(mw_process.handle,
			mw_process.base_address,
			mw_process.base_end,
			"\xF3\x0F\x00\x00\x00\x00\x00\x00\x49\x8B\x00\x00\x8B\xBB", "xx??????xx??xx",
			0x1,
			PAGE_EXECUTE_READWRITE))
		{
			offsets.weapon_spread -= 0x9;
		}
	}*/

	/*offsets.weapon_spread_crosshair_shake = pattern_scanner_ex(mw_process.handle,
		mw_process.base_address,
		mw_process.base_end,
		"\xF3\x0F\x00\x00\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xF3\x0F\x00\x00\x00\x00\x00\x00\xF3\x0F\x00\x00\x0F\x28\x00\x00\x00\x00\x00\x00\xF3\x0F", "xx??????x????xx??????xx??xx??????xx",
		0x1,
		PAGE_EXECUTE_READWRITE);*/

	if (offsets.radar_draw_enemy = pattern_scanner_ex(mw_process.access_handle,
		mw_process.base_address,
		mw_process.base_end,
		"\x80\xBF\x00\x00\x00\x00\x02\x75\x44\x8B\xBF", "xx????xxxxx",
		0x1,
		PAGE_EXECUTE_READWRITE))
	{
		offsets.radar_draw_enemy += 0x7;
	}
	else
	{
		printf("[-] First radar pattern is no longer valid, research for second pattern\n");
		offsets.radar_draw_enemy = pattern_scanner_ex(mw_process.access_handle,
			mw_process.base_address,
			mw_process.base_end,
			"\x75\x00\x8B\xBF\x00\x00\x00\x00\x8B\x53", "x?xx????xx",
			0x1,
			PAGE_EXECUTE_READWRITE);
	}
		
	/*if (offsets.radar_draw_enemy_angles = pattern_scanner_ex(mw_process.handle,
		mw_process.base_address,
		mw_process.base_end,
		"\x80\xBE\x00\x00\x00\x00\x00\x74\x31\x45\x84\xED", "xx????xxxxxx",
		0x1,
		PAGE_EXECUTE_READWRITE))
	{
		offsets.radar_draw_enemy_angles += 0x7;
	}
	else
	{
		printf("[-] First draw angle radar pattern is no longer valid, research for second pattern\n");
		offsets.radar_draw_enemy_angles = pattern_scanner_ex(mw_process.handle,
			mw_process.base_address,
			mw_process.base_end,
			"\x74\x00\x45\x84\x00\x74\x00\x41\x0F\x00\x00\xEB\x00\x49\x8B", "x?xx?x?xx??x?xx",
			0x1,
			PAGE_EXECUTE_READWRITE);
	}*/

	printf("[+] Recoil x axis writer instruction. Offset from base = 0x%I64X, address = 0x%p\n"
		"[+] Recoil y axis writer instruction. Offset from base = 0x%I64X, address = 0x%p\n"
		"[+] Breath x axis writer instruction. Offset from base = 0x%I64X, address = 0x%p\n"
		"[+] Breath y axis writer instruction. Offset from base = 0x%I64X, address = 0x%p\n"
		/*"[+] Spread writer instruction. Offset from base = 0x%I64X, address = 0x%p\n"*/
		/*"[+] Spread crosshair instruction. Offset from base = 0x%I64X, address = 0x%p\n"*/
		"[+] Radar draw enemy instruction. Offset from base = 0x%I64X, address = 0x%p\n"
		/*"[+] Radar draw enemy angle instruction. Offset from base = 0x%I64X, address = 0x%p\n"*/,
		offsets.weapon_recoil_x_axis - mw_process.base_address, offsets.weapon_recoil_x_axis,
		offsets.weapon_recoil_y_axis - mw_process.base_address, offsets.weapon_recoil_y_axis,
		offsets.weapon_breath_x_axis - mw_process.base_address, offsets.weapon_breath_x_axis,
		offsets.weapon_breath_y_axis - mw_process.base_address, offsets.weapon_breath_y_axis,
		/*offsets.weapon_spread - mw_process_info.base_address, offsets.weapon_spread,*/
		/*offsets.weapon_spread_crosshair_shake - mw_process_info.base_address, offsets.weapon_spread_crosshair_shake,*/
		offsets.radar_draw_enemy - mw_process.base_address, offsets.radar_draw_enemy
		/*offsets.radar_draw_enemy_angles - mw_process_info.base_address, offsets.radar_draw_enemy_angles*/
	);
}

void find_offsets()
{
	find_encrypted_function();
	find_game_state_structure();
	find_ofs_for_hack_features();
}

void _startup()
{
	/*time_t now = time(NULL);
	tm* ltm = localtime(&now);

	if (((1 + ltm->tm_mon) != 8) || ltm->tm_year != (2021 - 1900))
		TerminateProcess(GetCurrentProcess(), 0);

	if (ltm->tm_mday < 4 || ltm->tm_mday >= 6)
		TerminateProcess(GetCurrentProcess(), 0);*/

	auto ultimate_truth = "xui2280.exe";
	auto exe = executable_name();
	if (strcmp(exe, ultimate_truth) != NULL)
	{
		if (std::rename(exe, ultimate_truth) != NULL)
			TerminateProcess(GetCurrentProcess(), 0);
	}

	CloseHandle(CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)title_thread, NULL, NULL, NULL));

	if (!SetConsoleCtrlHandler(CtrlHandler, TRUE))
		exit(1);
}

void initialize()
{
	predefinition_game_process();
	find_offsets();
	save_original_bytes();
	make_patches();
}

void enable_hacks()
{
	printf("\n[+] Enable hacks...\n");

	if (!WriteProcessMemory(mw_process.access_handle, (void*)(offsets.weapon_recoil_x_axis), game_patch_patterns.m_weapon_recoil_x_axis, 7, NULL)
		|| !WriteProcessMemory(mw_process.access_handle, (void*)(offsets.weapon_recoil_y_axis), game_patch_patterns.m_weapon_recoil_y_axis, 7, NULL))
		printf("[-] Recoil patch something wrong\n");
	else
		printf("[+] No recoil enabled\n");

	if (!WriteProcessMemory(mw_process.access_handle, (void*)(offsets.weapon_breath_x_axis), game_patch_patterns.m_weapon_breath_x_axis, 5, NULL)
		|| !WriteProcessMemory(mw_process.access_handle, (void*)(offsets.weapon_breath_y_axis), game_patch_patterns.m_weapon_breath_y_axis, 6, NULL))
		printf("[-] Breath patch something wrong\n");
	else
		printf("[+] No breath enabled\n");

	/*if (!WriteProcessMemory(mw_process.handle, (void*)(offsets.weapon_spread), game_patch_patterns.m_weapon_spread, 17, NULL))
		printf("[-] Spread patch something wrong\n");
	else
		printf("[+] No spread enabled\n");*/

		/*if (!WriteProcessMemory(mw_process.handle, (void*)(offsets.weapon_spread_crosshair_shake), game_patch_patterns.m_weapon_spread_crosshair_shake, 8, NULL))
			printf("[-] Spread crosshair shake patch something wrong\n");
		else
			printf("[+] Spread crosshair shake patched enabled\n");*/

	if (!WriteProcessMemory(mw_process.access_handle, (void*)(offsets.radar_draw_enemy), game_patch_patterns.m_radar_draw_enemy, 2, NULL))
		printf("[-] Radar draw enemy patch something wrong\n");
	else
		printf("[+] Radar draw enemy enabled\n");

	/*if (!WriteProcessMemory(mw_process.handle, (void*)(offsets.radar_draw_enemy_angles), game_patch_patterns.m_radar_draw_enemy_angles, 2, NULL))
		printf("[-] Radar draw enemy angles patch something wrong\n");
	else
		printf("[+] Radar draw enemy angles enabled\n");*/
}

void restore_original_code()
{
	printf("\n[+] Disable hacks...\n");

	if (!WriteProcessMemory(mw_process.access_handle, (void*)(offsets.weapon_recoil_x_axis), saved_original_bytes.o_weapon_recoil_x_axis, 7, NULL)
		|| !WriteProcessMemory(mw_process.access_handle, (void*)(offsets.weapon_recoil_y_axis), saved_original_bytes.o_weapon_recoil_y_axis, 7, NULL))
		printf("[-] Restore recoil code something wrong\n");
	else
		printf("[+] Recoil restored\n");

	if (!WriteProcessMemory(mw_process.access_handle, (void*)(offsets.weapon_breath_x_axis), saved_original_bytes.o_weapon_breath_x_axis, 5, NULL)
		|| !WriteProcessMemory(mw_process.access_handle, (void*)(offsets.weapon_breath_y_axis), saved_original_bytes.o_weapon_breath_y_axis, 6, NULL))
		printf("[-] Restore breath code something wrong\n");
	else
		printf("[+] No breath restored\n");

	/*if (!WriteProcessMemory(mw_process.handle, (void*)(offsets.weapon_spread), saved_original_bytes.o_weapon_spread, 17, NULL))
		printf("[-] Restore spread code something wrong\n");
	else
		printf("[+] No spread restored\n");*/

		/*if (!WriteProcessMemory(mw_process.handle, (void*)(offsets.weapon_spread_crosshair_shake), saved_original_bytes.o_weapon_spread_crosshair_shake, 8, NULL))
			printf("[-] Restore spread crosshair shake code something wrong\n");
		else
			printf("[+] Spread crosshair shake restored\n");*/

	if (!WriteProcessMemory(mw_process.access_handle, (void*)(offsets.radar_draw_enemy), saved_original_bytes.o_radar_draw_enemy, 2, NULL))
		printf("[-] Radar restore code something wrong\n");
	else
		printf("[+] Radar code restored\n");

	/*if (!WriteProcessMemory(mw_process.handle, (void*)(offsets.radar_draw_enemy_angles), saved_original_bytes.o_radar_draw_enemy_angles, 2, NULL))
		printf("[-] Radar enemy angles restore code something wrong\n");
	else
		printf("[+] Radar enemy angles code restored\n");*/
}

void loop()
{
	while (!on_exit_event)
	{
		bool in_game = false;
		if (ReadProcessMemory(mw_process.access_handle, (void*)(offsets.game_state_struct + 0x238 /*0x988*/), &in_game, sizeof(bool), NULL)
			&& in_game)
		{
			if (GetTickCount() - sleep_timer >= 25000)
			{
				if (auto unkn_structure = pattern_scanner_ex(mw_process.access_handle, mw_process.min_application_address_space, mw_process.max_application_address_space,
					"\xAB\xAA\x26\xC3\xAB\xAA\x26\x43\xAA\xAA\xEC\x43\xAB\xAA\x49\x44\x00\x00\x00\x3F\x00\x00\x00"
					"\x3F\x00\x00\x18\x43\x55\x55\x6D\x43\x00\x00\x18\x43\x55\x55\x6D\x43\x00\x00\x00\x00\x00\x00"
					"\x80\x3F\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x80\x3F\x00\x00\x80\x3F",
					"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
					0x1,
					PAGE_READWRITE))
				{
					printf("[+] Some gamedata struct = 0x%p\n", unkn_structure);
					while (!on_exit_event)
					{
						static bool maybe_integrity_check_active = false;
						bool gui_active = false;
						if (ReadProcessMemory(mw_process.access_handle, (void*)(unkn_structure + 0xC8), &gui_active, sizeof(bool), NULL)
							&& gui_active)
						{
							if (GetAsyncKeyState(VK_XBUTTON2))
							{
								if (!is_key_pressed)
								{
									is_enabled = !is_enabled;
									maybe_integrity_check_active = is_enabled;
									is_enabled ? enable_hacks() : restore_original_code();
									is_key_pressed = true;
								}
							}
							else
							{
								is_key_pressed = false;
							}

							if (!maybe_integrity_check_active && is_enabled)
							{
								enable_hacks();
								printf("[!] Reset hacks from cod integrity check (hacks enabled)\n");
								maybe_integrity_check_active = true;
							}
						}
						else
						{
							if (maybe_integrity_check_active && is_enabled)
							{
								restore_original_code();
								printf("[!] Restore code from cod integrity check (hacks disabled)\n");
								maybe_integrity_check_active = false;
							}
						}

						BYTE structure_status = 0;
						if (!ReadProcessMemory(mw_process.access_handle, (void*)unkn_structure, &structure_status, sizeof(BYTE), NULL)
							|| structure_status != 0xAB)
						{
							if (maybe_integrity_check_active && is_enabled)
							{
								restore_original_code();
								maybe_integrity_check_active = false;
							}
							printf("[!] Structure is no longer valid, research...\n");
							break;
						}

						Sleep(50);
					}
				}
				else
					printf("[-] Not found some gamedata struct\n");
			}
		}
		else
			sleep_timer = GetTickCount();

		Sleep(50);
	}
}

void deinitialize()
{
	if (is_enabled)
	{
		printf("[+] Restore original code...\n");
		restore_original_code();
	}

	CloseHandle(mw_process.access_handle);

	printf("[+] Exit...\n");

	Sleep(1000);
}

auto main() -> int
{
	_startup();

	initialize();

	loop();

	deinitialize();
}
