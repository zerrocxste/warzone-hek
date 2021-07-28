#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <psapi.h>

struct
{
	DWORD pid;
	DWORD_PTR base_address;
	DWORD_PTR base_size;
	DWORD_PTR base_end;
	HANDLE handle;
} mw_process_info;

struct
{
	DWORD_PTR prologue_encrypted_function;
	DWORD_PTR game_state_struct;
	DWORD_PTR weapon_recoil_x_axis;
	DWORD_PTR weapon_recoil_y_axis;
	DWORD_PTR weapon_breath_x_axis;
	DWORD_PTR weapon_breath_y_axis;
	DWORD_PTR weapon_spread;
	//DWORD_PTR weapon_spread_crosshair_shake;
} offsets;

struct 
{
	BYTE* o_weapon_recoil_x_axis;
	BYTE* o_weapon_recoil_y_axis;
	BYTE* o_weapon_breath_x_axis;
	BYTE* o_weapon_breath_y_axis;
	BYTE* o_weapon_spread;
	BYTE* o_weapon_spread_crosshair_shake;
} saved_original_bytes;

struct
{
	BYTE* m_weapon_recoil_x_axis;
	BYTE* m_weapon_recoil_y_axis;
	BYTE* m_weapon_breath_x_axis;
	BYTE* m_weapon_breath_y_axis;
	BYTE* m_weapon_spread;
	BYTE* m_weapon_spread_crosshair_shake;
} game_patch_patterns;

bool on_exit_event = false;

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

			//if (IsBadCodePtr((FARPROC)(base + i + j)) != NULL)
			//	continue;

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

	while (start < end && VirtualQueryEx(handle, (void*)start, &mbi, sizeof(MEMORY_BASIC_INFORMATION)) != NULL)
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

DWORD_PTR asm64_solve_dest(DWORD64 src, DWORD relative_address)
{
	auto dest = src + relative_address;
	auto rel64 = (DWORD)src + (DWORD64)relative_address;
	auto rel32 = (DWORD)dest;
	return dest - (rel64 - rel32);
}

void save_original_bytes()
{
	saved_original_bytes.o_weapon_recoil_x_axis = new BYTE[7];
	saved_original_bytes.o_weapon_recoil_y_axis = new BYTE[7];
	saved_original_bytes.o_weapon_breath_x_axis = new BYTE[5];
	saved_original_bytes.o_weapon_breath_y_axis = new BYTE[6];
	saved_original_bytes.o_weapon_spread = new BYTE[17];
	saved_original_bytes.o_weapon_spread_crosshair_shake = new BYTE[8];

	ReadProcessMemory(mw_process_info.handle, (void*)offsets.weapon_recoil_x_axis, saved_original_bytes.o_weapon_recoil_x_axis, 7, NULL) ?
		printf("[+] Recoil x axis instruction dumped\n") : printf("[+] Recoil x axis instruction dump failed\n");
		
	ReadProcessMemory(mw_process_info.handle, (void*)offsets.weapon_recoil_y_axis, saved_original_bytes.o_weapon_recoil_y_axis, 7, NULL) ?
		printf("[+] Recoil y axis instruction dumped\n") : printf("[+] Recoil y axis instruction dump failed\n");

	ReadProcessMemory(mw_process_info.handle, (void*)offsets.weapon_breath_x_axis, saved_original_bytes.o_weapon_breath_x_axis, 5, NULL) ?
		printf("[+] Breath x axis instruction dumped\n") : printf("[+] Breath x axis instruction dump failed\n");

	ReadProcessMemory(mw_process_info.handle, (void*)offsets.weapon_breath_y_axis, saved_original_bytes.o_weapon_breath_y_axis, 6, NULL) ?
		printf("[+] Breath y axis instruction dumped\n") : printf("[+] Breath y axis instruction dump failed\n");

	ReadProcessMemory(mw_process_info.handle, (void*)offsets.weapon_spread, saved_original_bytes.o_weapon_spread, 17, NULL) ?
		printf("[+] Weapon spread instruction dumped\n") : printf("[+] Weapon spread instruction dump failed\n");

	/*ReadProcessMemory(mw_process_info.handle, (void*)offsets.weapon_spread_crosshair_shake, saved_original_bytes.o_weapon_spread_crosshair_shake, 8, NULL) ?
		printf("[+] Weapon spread crosshair shake instruction dumped\n") : printf("[+] Weapon spread crosshair shake instruction dump failed\n");*/
}

void make_patches()
{
	game_patch_patterns.m_weapon_recoil_x_axis = new BYTE[7];
	game_patch_patterns.m_weapon_recoil_y_axis = new BYTE[7];
	game_patch_patterns.m_weapon_breath_x_axis = new BYTE[5];
	game_patch_patterns.m_weapon_breath_y_axis = new BYTE[6];
	game_patch_patterns.m_weapon_spread = new BYTE[17];
	game_patch_patterns.m_weapon_spread_crosshair_shake = new BYTE[8];

	memset(game_patch_patterns.m_weapon_recoil_x_axis, 0x90, 7);
	memset(game_patch_patterns.m_weapon_recoil_y_axis, 0x90, 7);
	memset(game_patch_patterns.m_weapon_breath_x_axis, 0x90, 5);
	memset(game_patch_patterns.m_weapon_breath_y_axis, 0x90, 6);
	memcpy(game_patch_patterns.m_weapon_spread, "\xC7\x83\x8C\x07\x00\x00\x00\x00\x16\xC3\x90\x90\x90\x90\x90\x90\x90", 17);
	memcpy((void*)(game_patch_patterns.m_weapon_spread + 0x2), (void*)(saved_original_bytes.o_weapon_spread + 0xD), 4);
	//set spread amooooooooooount =pppp
	float spread_value = -150.f; //better value for all weaponz, strelyaet pochti vse i tochky
	memcpy((void*)(game_patch_patterns.m_weapon_spread + 0x6), &spread_value, sizeof(float));
	memset(game_patch_patterns.m_weapon_spread_crosshair_shake, 0x90, 8);
}

void find_encrypted_function()
{
	auto e8_encrypted_func = pattern_scanner_ex(mw_process_info.handle, mw_process_info.base_address, mw_process_info.base_end,
		"\xE8\x00\x00\x00\x00\x48\x8D\x00\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x44\x8B\x00\x00\x00\x00\x00\x48\x8D\x00\x00\x00\x00\x00\x4C\x8D", "x????xx?????x????xx?????xx?????xx",
		0x1, PAGE_EXECUTE_READWRITE);

	if (!e8_encrypted_func)
	{
		printf("[-] Not found required instruction\n");
		system("pause");
		exit(1);
	}

	printf("[+] Found encrypted function caller = 0x%p\n", e8_encrypted_func);

	DWORD relative_address_encrypted_function = 0x0;
	ReadProcessMemory(mw_process_info.handle, (void*)(e8_encrypted_func + 0x1), &relative_address_encrypted_function, sizeof(DWORD), NULL);

	printf("[+] Relative offset to enctypted function from E8 XXXX = 0x%X\n", relative_address_encrypted_function);

	offsets.prologue_encrypted_function = asm64_solve_dest(e8_encrypted_func + 0x5, relative_address_encrypted_function);

	printf("[+] Encrypted function found = 0x%p\n", offsets.prologue_encrypted_function);
}

void find_game_state_structure()
{
	auto func_7FF73E4C1A40 = pattern_scanner_ex(mw_process_info.handle, mw_process_info.base_address, mw_process_info.base_end,
		"\x48\x89\x00\x00\x00\x57\x48\x83\xEC\x00\x48\x8B\x00\x4C\x8D\x00\x00\x00\x00\x00\x48\x8B\x00\xBA\x00\x00\x00\x00\x48\x8B\x00\xE8\x00\x00\x00\x00\x4C\x8D",
		"xx???xxxx?xx?xx?????xx?x????xx?x????xx",
		0x1, PAGE_EXECUTE_READWRITE);

	if (!func_7FF73E4C1A40)
	{
		//"48 8B ? ? ? ? ? BA ? ? ? ? 48 8B ? E8 ? ? ? ? 48 8B ? 48 85 ? 75 ? 33 DB EB ? 0F B7 ? B9 ? ? ? ? 66 3B ? 73"
		func_7FF73E4C1A40 = pattern_scanner_ex(mw_process_info.handle, mw_process_info.base_address, mw_process_info.base_end,
			"\x48\x8B\x00\x00\x00\x00\x00\xBA\x00\x00\x00\x00\x48\x8B\x00\xE8\x00\x00\x00\x00\x48\x8B\x00\x48\x85\x00\x75\x00\x33\xDB\xEB\x00\x0F\xB7\x00\xB9\x00\x00\x00\x00\x66\x3B\x00\x73",
			"xx?????x????xx?x????xx?xx?x?xxx?xx?x????xx?x",
			0x1, PAGE_EXECUTE_READWRITE);

		if (!func_7FF73E4C1A40)
		{
			printf("[-] Not found required function\n");
			system("pause");
			exit(1);
		}
	}
	else
		func_7FF73E4C1A40 += 0x48;

	DWORD relative_address = 0x0;
	ReadProcessMemory(mw_process_info.handle, (void*)(func_7FF73E4C1A40 + 0x3), &relative_address, sizeof(DWORD), NULL);

	ReadProcessMemory(mw_process_info.handle, (void*)asm64_solve_dest(func_7FF73E4C1A40 + 0x7, relative_address), &offsets.game_state_struct, sizeof(DWORD_PTR), NULL);

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
	if (offsets.prologue_encrypted_function)
	{
		if (offsets.weapon_recoil_x_axis = pattern_scanner_ex(mw_process_info.handle, 
			offsets.prologue_encrypted_function, 
			mw_process_info.base_end,
			"\x8B\x06\x41\x89\x80", "xxxxx",
			0x1,
			PAGE_EXECUTE_READWRITE))
		{
			offsets.weapon_recoil_x_axis += 0x2;

			offsets.weapon_recoil_y_axis = pattern_scanner_ex(mw_process_info.handle,
				offsets.weapon_recoil_x_axis + 0x1,
				offsets.weapon_recoil_x_axis + 0x50,
				"\x41\x89\x80", "xxx",
				0x1,
				PAGE_EXECUTE_READWRITE);
		}
	}

	offsets.weapon_breath_x_axis = pattern_scanner_ex(mw_process_info.handle, 
		mw_process_info.base_address, 
		mw_process_info.base_end,
		"\xF3\x44\x00\x00\x00\xF3\x44\x00\x00\x00\x00\xF3\x44\x00\x00\x00\x00\x48\x8B\x00\x00\x00\x00\x00\x48\x33", "xx???xx????xx????xx?????xx",
		0x1,
		PAGE_EXECUTE_READWRITE);

	offsets.weapon_breath_y_axis = pattern_scanner_ex(mw_process_info.handle, 
		mw_process_info.base_address, 
		mw_process_info.base_end,
		"\xF3\x44\x00\x00\x00\x00\xF3\x44\x00\x00\x00\x00\x48\x8B\x00\x00\x00\x00\x00\x48\x33", "xx????xx????xx?????xx",
		0x1,
		PAGE_EXECUTE_READWRITE);

	offsets.weapon_spread = pattern_scanner_ex(mw_process_info.handle, 
		mw_process_info.base_address, 
		mw_process_info.base_end,
		"\x44\x0F\x00\x00\x00\x00\x00\x00\x00\xF3\x0F\x00\x00\x00\x00\x00\x00\x49\x8B", "xx???????xx??????xx",
		0x1,
		PAGE_EXECUTE_READWRITE);
	//structure offset to += 0x13

	if (offsets.weapon_spread == NULL)
	{
		printf("[-] First spread pattern is no longer valid, research for second pattern\n");

		offsets.weapon_spread = pattern_scanner_ex(mw_process_info.handle, 
			mw_process_info.base_address, 
			mw_process_info.base_end,
			"\xF3\x0F\x00\x00\x00\x00\x00\x00\x49\x8B\x00\x00\x8B\xBB", "xx??????xx??xx",
			0x1,
			PAGE_EXECUTE_READWRITE);

		if (offsets.weapon_spread != NULL)
			offsets.weapon_spread -= 0x9;
	}

	/*offsets.weapon_spread_crosshair_shake = pattern_scanner_ex(mw_process_info.handle,
		mw_process_info.base_address,
		mw_process_info.base_end,
		"\xF3\x0F\x00\x00\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xF3\x0F\x00\x00\x00\x00\x00\x00\xF3\x0F\x00\x00\x0F\x28\x00\x00\x00\x00\x00\x00\xF3\x0F", "xx??????x????xx??????xx??xx??????xx",
		0x1,
		PAGE_EXECUTE_READWRITE);*/

	printf("[+] Recoil x axis writer instruction. Offset from base = 0x%I64X, address = 0x%p\n"
		"[+] Recoil y axis writer instruction. Offset from base = 0x%I64X, address = 0x%p\n"
		"[+] Breath x axis writer instruction. Offset from base = 0x%I64X, address = 0x%p\n"
		"[+] Breath y axis writer instruction. Offset from base = 0x%I64X, address = 0x%p\n"
		"[+] Spread writer instruction. Offset from base = 0x%I64X, address = 0x%p\n"
		/*"[+] Spread crosshair instruction. Offset from base = 0x%I64X, address = 0x%p\n"*/,
		offsets.weapon_recoil_x_axis - mw_process_info.base_address, offsets.weapon_recoil_x_axis,
		offsets.weapon_recoil_y_axis - mw_process_info.base_address, offsets.weapon_recoil_y_axis,
		offsets.weapon_breath_x_axis - mw_process_info.base_address, offsets.weapon_breath_x_axis,
		offsets.weapon_breath_y_axis - mw_process_info.base_address, offsets.weapon_breath_y_axis,
		offsets.weapon_spread - mw_process_info.base_address, offsets.weapon_spread/*,
		offsets.weapon_spread_crosshair_shake - mw_process_info.base_address, offsets.weapon_spread_crosshair_shake*/
	);
}

void find_offsets()
{
	find_encrypted_function();
	find_game_state_structure();
	find_ofs_for_hack_features();
}

void initialize()
{
	find_offsets();
	save_original_bytes();
	make_patches();
}

void enable_hacks()
{
	printf("\n[+] Enable hacks...\n");

	if (!WriteProcessMemory(mw_process_info.handle, (void*)(offsets.weapon_recoil_x_axis), game_patch_patterns.m_weapon_recoil_x_axis, 7, NULL)
		|| !WriteProcessMemory(mw_process_info.handle, (void*)(offsets.weapon_recoil_y_axis), game_patch_patterns.m_weapon_recoil_y_axis, 7, NULL))
		printf("[-] Recoil patch something wrong\n");
	else
		printf("[+] No recoil enabled\n");

	if (!WriteProcessMemory(mw_process_info.handle, (void*)(offsets.weapon_breath_x_axis), game_patch_patterns.m_weapon_breath_x_axis, 5, NULL)
		|| !WriteProcessMemory(mw_process_info.handle, (void*)(offsets.weapon_breath_y_axis), game_patch_patterns.m_weapon_breath_y_axis, 6, NULL))
		printf("[-] Breath patch something wrong\n");
	else
		printf("[+] No breath enabled\n");

	if (!WriteProcessMemory(mw_process_info.handle, (void*)(offsets.weapon_spread), game_patch_patterns.m_weapon_spread, 17, NULL))
		printf("[-] Spread patch something wrong\n");
	else
		printf("[+] No spread enabled\n");

	/*if (!WriteProcessMemory(mw_process_info.handle, (void*)(offsets.weapon_spread_crosshair_shake), game_patch_patterns.m_weapon_spread_crosshair_shake, 8, NULL))
		printf("[-] Spread crosshair shake patch something wrong\n");
	else
		printf("[+] Spread crosshair shake patched enabled\n");*/
}

void restore_original_code()
{
	printf("\n[+] Disable hacks...\n");

	if (!WriteProcessMemory(mw_process_info.handle, (void*)(offsets.weapon_recoil_x_axis), saved_original_bytes.o_weapon_recoil_x_axis, 7, NULL)
		|| !WriteProcessMemory(mw_process_info.handle, (void*)(offsets.weapon_recoil_y_axis), saved_original_bytes.o_weapon_recoil_y_axis, 7, NULL))
		printf("[-] Restore recoil code something wrong\n");
	else
		printf("[+] Recoil restored\n");

	if (!WriteProcessMemory(mw_process_info.handle, (void*)(offsets.weapon_breath_x_axis), saved_original_bytes.o_weapon_breath_x_axis, 5, NULL)
		|| !WriteProcessMemory(mw_process_info.handle, (void*)(offsets.weapon_breath_y_axis), saved_original_bytes.o_weapon_breath_y_axis, 6, NULL))
		printf("[-] Restore breath code something wrong\n");
	else
		printf("[+] No breath restored\n");

	if (!WriteProcessMemory(mw_process_info.handle, (void*)(offsets.weapon_spread), saved_original_bytes.o_weapon_spread, 17, NULL))
		printf("[-] Restore spread code something wrong\n");
	else
		printf("[+] No spread restored\n");

	/*if (!WriteProcessMemory(mw_process_info.handle, (void*)(offsets.weapon_spread_crosshair_shake), saved_original_bytes.o_weapon_spread_crosshair_shake, 8, NULL))
		printf("[-] Restore spread crosshair shake code something wrong\n");
	else
		printf("[+] Spread crosshair shake restored\n");*/
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

int main()
{
	SetConsoleTitle("Microsoft Word");

	if (!SetConsoleCtrlHandler(CtrlHandler, TRUE))
		return 0;

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)title_thread, NULL, NULL, NULL);

	auto process_name = "ModernWarfare.exe";

	auto process = get_process(process_name);

	if (!process)
	{
		printf("[-] Not found MW19 process\n");
		system("pause");
		exit(1);
	}

	mw_process_info.pid = process->th32ProcessID;

	auto base = find_module(mw_process_info.pid, process_name);

	if (!base)
	{
		printf("[-] Not found MW19 module\n");
		system("pause");
		exit(1);
	}

	mw_process_info.base_address = (DWORD_PTR)base->modBaseAddr;
	mw_process_info.base_size = (DWORD_PTR)base->modBaseSize;
	mw_process_info.base_end = mw_process_info.base_address + mw_process_info.base_size;

	printf("[+] Found MW19 process. pid = %d, base = 0x%p, base size = %I64X, base end = %p\n", 
		mw_process_info.pid, 
		mw_process_info.base_address, 
		mw_process_info.base_size, 
		mw_process_info.base_end);

	mw_process_info.handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, mw_process_info.pid);

	if (!mw_process_info.handle)
	{
		printf("[-] Failed open process\n");
		system("pause");
		exit(1);
	}

	initialize();

	SYSTEM_INFO info{};
	GetSystemInfo(&info);

	DWORD_PTR start = (DWORD_PTR)info.lpMinimumApplicationAddress; //dada etoj drugoe prilojenie bla bla bla......
	DWORD_PTR end = (DWORD_PTR)info.lpMaximumApplicationAddress;

	DWORD sleep_timer = 0x0;

	bool is_enabled = false;
	bool is_key_pressed = false;

	while (!on_exit_event)
	{
		bool in_game = false; 
		if (ReadProcessMemory(mw_process_info.handle, (void*)(offsets.game_state_struct + 0x238 /*0x988*/), &in_game, sizeof(bool), NULL) 
			&& in_game)
		{
			if (GetTickCount() - sleep_timer >= 25000)
			{
				auto unkn_structure = pattern_scanner_ex(mw_process_info.handle, start, end,
					"\xAB\xAA\x26\xC3\xAB\xAA\x26\x43\xAA\xAA\xEC\x43\xAB\xAA\x49\x44\x00\x00\x00\x3F\x00\x00\x00"
					"\x3F\x00\x00\x18\x43\x55\x55\x6D\x43\x00\x00\x18\x43\x55\x55\x6D\x43\x00\x00\x00\x00\x00\x00"
					"\x80\x3F\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x80\x3F\x00\x00\x80\x3F",
					"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
					0x1,
					PAGE_READWRITE);

				if (unkn_structure != NULL)
				{
					printf("[+] Some gamedata struct = 0x%p\n", unkn_structure);
					while (!on_exit_event)
					{
						static bool maybe_integrity_check_active = false;
						bool gui_active = false;
						if (ReadProcessMemory(mw_process_info.handle, (void*)(unkn_structure + 0xC8), &gui_active, sizeof(bool), NULL)
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

						BYTE structure_status = 0x0;
						if (!ReadProcessMemory(mw_process_info.handle, (void*)unkn_structure, &structure_status, sizeof(BYTE), NULL)
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

	if (is_enabled)
	{
		printf("[+] Restore original code...\n");
		restore_original_code();
	}

	CloseHandle(mw_process_info.handle);

	printf("[+] Exit...\n");

	Sleep(1000);
}
