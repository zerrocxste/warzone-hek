#include "../includes.h"

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
	DWORD_PTR radar_draw_enemy;
} offsets;

struct 
{
	BYTE* o_weapon_recoil_x_axis;
	BYTE* o_weapon_recoil_y_axis;
	BYTE* o_weapon_breath_x_axis;
	BYTE* o_weapon_breath_y_axis;
	BYTE* o_radar_draw_enemy;
} saved_original_bytes;

struct
{
	BYTE* m_weapon_recoil_x_axis;
	BYTE* m_weapon_recoil_y_axis;
	BYTE* m_weapon_breath_x_axis;
	BYTE* m_weapon_breath_y_axis;
	BYTE* m_radar_draw_enemy;
} game_patch_patterns;

bool is_enabled = false;

bool predefinition_game_process()
{
	auto process_name = "ModernWarfare.exe";

	auto process = utilites::get_process(process_name);

	if (!process)
	{
		printf("[-] Not found MW19 process\n");
		return false;
	}

	mw_process.pid = process->th32ProcessID;

	auto base = utilites::find_module(mw_process.pid, process_name);

	if (!base)
	{
		printf("[-] Not found MW19 module\n");
		return false;
	}

	mw_process.base_address = (DWORD_PTR)base->modBaseAddr;
	mw_process.base_size = (DWORD_PTR)base->modBaseSize;
	mw_process.base_end = mw_process.base_address + mw_process.base_size;

	printf("[+] Found MW19 process. pid = %d, base = 0x%p, base size = %I64X, base end = 0x%p\n",
		mw_process.pid,
		mw_process.base_address,
		mw_process.base_size,
		mw_process.base_end);

	mw_process.access_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, mw_process.pid);

	if (!mw_process.access_handle)
	{
		printf("[-] Failed open process\n");
		return false;
	}

	SYSTEM_INFO info{};
	GetSystemInfo(&info);

	mw_process.min_application_address_space = (DWORD_PTR)info.lpMinimumApplicationAddress;
	mw_process.max_application_address_space = (DWORD_PTR)info.lpMaximumApplicationAddress;

	return true;
}

bool save_original_bytes()
{
	saved_original_bytes.o_weapon_recoil_x_axis = new BYTE[7];
	saved_original_bytes.o_weapon_recoil_y_axis = new BYTE[7];
	saved_original_bytes.o_weapon_breath_x_axis = new BYTE[5];
	saved_original_bytes.o_weapon_breath_y_axis = new BYTE[6];
	saved_original_bytes.o_radar_draw_enemy = new BYTE[2];

	if (!saved_original_bytes.o_weapon_recoil_x_axis ||
		!saved_original_bytes.o_weapon_recoil_y_axis ||
		!saved_original_bytes.o_weapon_breath_x_axis ||
		!saved_original_bytes.o_weapon_breath_y_axis ||
		!saved_original_bytes.o_radar_draw_enemy)
	{
		printf("[-] Failed allocate memory for save original bytes\n");
		return false;
	}

	auto _read_status1 = ReadProcessMemory(mw_process.access_handle, (void*)offsets.weapon_recoil_x_axis, saved_original_bytes.o_weapon_recoil_x_axis, 7, NULL);
	auto _read_status2 = ReadProcessMemory(mw_process.access_handle, (void*)offsets.weapon_recoil_y_axis, saved_original_bytes.o_weapon_recoil_y_axis, 7, NULL);
	auto _read_status3 = ReadProcessMemory(mw_process.access_handle, (void*)offsets.weapon_breath_x_axis, saved_original_bytes.o_weapon_breath_x_axis, 5, NULL);
	auto _read_status4 = ReadProcessMemory(mw_process.access_handle, (void*)offsets.weapon_breath_y_axis, saved_original_bytes.o_weapon_breath_y_axis, 6, NULL);
	auto _read_status5 = ReadProcessMemory(mw_process.access_handle, (void*)offsets.radar_draw_enemy, saved_original_bytes.o_radar_draw_enemy, 2, NULL);

	_read_status1 ? printf("[+] Recoil x axis instruction dumped\n") : printf("[-] Recoil x axis instruction dump failed\n");
	_read_status2 ? printf("[+] Recoil y axis instruction dumped\n") : printf("[-] Recoil y axis instruction dump failed\n");
	_read_status3 ? printf("[+] Breath x axis instruction dumped\n") : printf("[-] Breath x axis instruction dump failed\n");
	_read_status4 ? printf("[+] Breath y axis instruction dumped\n") : printf("[-] Breath y axis instruction dump failed\n");
	_read_status5 ? printf("[+] Radar enemy check instruction dumped\n") : printf("[-] Radar enemy check instruction dump failed\n");

	return _read_status1 && _read_status2 && _read_status3 && _read_status4 && _read_status5;
}

bool make_patches()
{
	game_patch_patterns.m_weapon_recoil_x_axis = new BYTE[7];
	game_patch_patterns.m_weapon_recoil_y_axis = new BYTE[7];
	game_patch_patterns.m_weapon_breath_x_axis = new BYTE[5];
	game_patch_patterns.m_weapon_breath_y_axis = new BYTE[6];
	game_patch_patterns.m_radar_draw_enemy = new BYTE[2];

	if (!game_patch_patterns.m_weapon_recoil_x_axis ||
		!game_patch_patterns.m_weapon_recoil_y_axis ||
		!game_patch_patterns.m_weapon_breath_x_axis ||
		!game_patch_patterns.m_weapon_breath_y_axis ||
		!game_patch_patterns.m_radar_draw_enemy)
	{
		printf("[-] Failed allocate memory for make patches\n");
		return false;
	}

	memset(game_patch_patterns.m_weapon_recoil_x_axis, 0x90, 7);
	memset(game_patch_patterns.m_weapon_recoil_y_axis, 0x90, 7);
	memset(game_patch_patterns.m_weapon_breath_x_axis, 0x90, 5);
	memset(game_patch_patterns.m_weapon_breath_y_axis, 0x90, 6);
	memset(game_patch_patterns.m_radar_draw_enemy, 0x90, 2);

	return true;
}

bool find_encrypted_function()
{
	auto e8xxxx_encrypted_func = utilites::pattern_scanner_ex(mw_process.access_handle,
		mw_process.base_address, mw_process.base_end,
		"\xE8\x00\x00\x00\x00\x48\x8D\x00\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x44\x8B\x00\x00\x00\x00\x00\x48\x8D\x00\x00\x00\x00\x00\x4C\x8D", "x????xx?????x????xx?????xx?????xx",
		0x1, 
		PAGE_EXECUTE_READWRITE);

	if (!e8xxxx_encrypted_func)
	{
		printf("[-] Not found required instruction\n");
		return false;
	}

	printf("[+] Found encrypted function caller = 0x%p\n", e8xxxx_encrypted_func);

	DWORD rva_encrypted_function = 0;
	ReadProcessMemory(mw_process.access_handle, (void*)(e8xxxx_encrypted_func + 0x1), &rva_encrypted_function, sizeof(DWORD), NULL);

	printf("[+] Relative offset to encrypted function from E8 XXXX = 0x%X\n", rva_encrypted_function);

	offsets.prologue_encrypted_function = utilites::asm64_solve_dest(e8xxxx_encrypted_func + 0x5, rva_encrypted_function);

	printf("[+] Encrypted function found = 0x%p\n", offsets.prologue_encrypted_function);

	return true;
}

bool find_game_state_structure()
{
	//"48 8B ? ? ? ? ? BA ? ? ? ? 48 8B ? E8 ? ? ? ? 48 8B ? 48 85 ? 75 ? 33 DB EB ? 0F B7 ? B9 ? ? ? ? 66 3B ? 73"
	auto __mov_rbx_pofxxxx__va = utilites::pattern_scanner_ex(mw_process.access_handle, 
		mw_process.base_address, mw_process.base_end,
		"\x48\x8B\x00\x00\x00\x00\x00\xBA\x00\x00\x00\x00\x48\x8B\x00\xE8\x00\x00\x00\x00\x48\x8B\x00\x48\x85\x00\x75\x00\x33\xDB\xEB\x00\x0F\xB7\x00\xB9\x00\x00\x00\x00\x66\x3B\x00\x73",
		"xx?????x????xx?x????xx?xx?x?xxx?xx?x????xx?x",
		0x1, 
		PAGE_EXECUTE_READWRITE);

	if (!__mov_rbx_pofxxxx__va)
	{
		printf("[-] Not found required instruction\n");
		return false;
	}

	DWORD rva_game_state_struct = 0;
	ReadProcessMemory(mw_process.access_handle, (void*)(__mov_rbx_pofxxxx__va + 0x3), &rva_game_state_struct, sizeof(DWORD), NULL);

	ReadProcessMemory(mw_process.access_handle, (void*)utilites::asm64_solve_dest(__mov_rbx_pofxxxx__va + 0x7, rva_game_state_struct), &offsets.game_state_struct, sizeof(DWORD_PTR), NULL);

	if (!offsets.game_state_struct)
	{
		printf("[-] Not found game state struct\n");
		return false;
	}

	printf("[+] Game state struct = 0x%p\n", offsets.game_state_struct);

	return true;
}

bool find_va_for_hack_features()
{
	if (offsets.prologue_encrypted_function && 
		(offsets.weapon_recoil_x_axis = utilites::pattern_scanner_ex(mw_process.access_handle,
		offsets.prologue_encrypted_function, 
		mw_process.base_end,
		"\x8B\x06\x41\x89\x80", "xxxxx",
		0x1,
		PAGE_EXECUTE_READWRITE)))
	{
		offsets.weapon_recoil_x_axis += 0x2;

		offsets.weapon_recoil_y_axis = utilites::pattern_scanner_ex(mw_process.access_handle,
			offsets.weapon_recoil_x_axis + 0x1,
			offsets.weapon_recoil_x_axis + 0x50,
			"\x41\x89\x80", "xxx",
			0x1,
			PAGE_EXECUTE_READWRITE);
	}

	offsets.weapon_breath_x_axis = utilites::pattern_scanner_ex(mw_process.access_handle,
		mw_process.base_address,
		mw_process.base_end,
		"\xF3\x44\x00\x00\x00\xF3\x44\x00\x00\x00\x00\xF3\x44\x00\x00\x00\x00\x48\x8B\x00\x00\x00\x00\x00\x48\x33", "xx???xx????xx????xx?????xx",
		0x1,
		PAGE_EXECUTE_READWRITE);

	offsets.weapon_breath_y_axis = utilites::pattern_scanner_ex(mw_process.access_handle,
		mw_process.base_address,
		mw_process.base_end,
		"\xF3\x44\x00\x00\x00\x00\xF3\x44\x00\x00\x00\x00\x48\x8B\x00\x00\x00\x00\x00\x48\x33", "xx????xx????xx?????xx",
		0x1,
		PAGE_EXECUTE_READWRITE);

	if (offsets.radar_draw_enemy = utilites::pattern_scanner_ex(mw_process.access_handle,
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
		offsets.radar_draw_enemy = utilites::pattern_scanner_ex(mw_process.access_handle,
			mw_process.base_address,
			mw_process.base_end,
			"\x75\x00\x8B\xBF\x00\x00\x00\x00\x8B\x53", "x?xx????xx",
			0x1,
			PAGE_EXECUTE_READWRITE);
	}

	printf(
		"[+] Recoil x axis writer instruction: VA = 0x%p, RVA from base = 0x%I64X\n"
		"[+] Recoil y axis writer instruction: VA = 0x%p, RVA from base = 0x%I64X\n"
		"[+] Breath x axis writer instruction: VA = 0x%p, RVA from base = 0x%I64X\n"
		"[+] Breath y axis writer instruction: VA = 0x%p, RVA from base = 0x%I64X\n"
		"[+] Radar draw enemy instruction: VA = 0x%p, RVA from base = 0x%I64X\n",
		offsets.weapon_recoil_x_axis, offsets.weapon_recoil_x_axis - mw_process.base_address,
		offsets.weapon_recoil_y_axis, offsets.weapon_recoil_y_axis - mw_process.base_address,
		offsets.weapon_breath_x_axis, offsets.weapon_breath_x_axis - mw_process.base_address,
		offsets.weapon_breath_y_axis, offsets.weapon_breath_y_axis - mw_process.base_address,
		offsets.radar_draw_enemy, offsets.radar_draw_enemy - mw_process.base_address
	);

	return offsets.weapon_recoil_x_axis &&
		offsets.weapon_recoil_y_axis &&
		offsets.weapon_breath_x_axis &&
		offsets.weapon_breath_y_axis &&
		offsets.radar_draw_enemy;
}													

bool find_offsets()
{
	if (!find_encrypted_function())
		return false;

	if (!find_game_state_structure())
		return false;

	if (!find_va_for_hack_features())
		return false;

	return true;
}

bool initialize()
{
	if (!predefinition_game_process())
		return false;

	if (!find_offsets())
		return false;

	if (!save_original_bytes())
		return false;

	if (!make_patches())
		return false;

	return true;
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

	if (!WriteProcessMemory(mw_process.access_handle, (void*)(offsets.radar_draw_enemy), game_patch_patterns.m_radar_draw_enemy, 2, NULL))
		printf("[-] Radar draw enemy patch something wrong\n");
	else
		printf("[+] Radar draw enemy enabled\n");
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

	if (!WriteProcessMemory(mw_process.access_handle, (void*)(offsets.radar_draw_enemy), saved_original_bytes.o_radar_draw_enemy, 2, NULL))
		printf("[-] Radar restore code something wrong\n");
	else
		printf("[+] Radar code restored\n");
}

void loop()
{
	DWORD sleep_timer = 0;
	bool is_key_pressed = false;
	while (!console_app_handler::m_on_exit_event)
	{
		bool in_game = false;
		if (ReadProcessMemory(mw_process.access_handle, (void*)(offsets.game_state_struct + 0x238 /*0x988*/), &in_game, sizeof(bool), NULL)
			&& in_game)
		{
			if (GetTickCount() - sleep_timer >= 25000)
			{
				if (auto unkn_structure = utilites::pattern_scanner_ex(mw_process.access_handle, mw_process.min_application_address_space, mw_process.max_application_address_space,
					"\xAB\xAA\x26\xC3\xAB\xAA\x26\x43\xAA\xAA\xEC\x43\xAB\xAA\x49\x44\x00\x00\x00\x3F\x00\x00\x00"
					"\x3F\x00\x00\x18\x43\x55\x55\x6D\x43\x00\x00\x18\x43\x55\x55\x6D\x43\x00\x00\x00\x00\x00\x00"
					"\x80\x3F\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x80\x3F\x00\x00\x80\x3F",
					"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
					0x1,
					PAGE_READWRITE))
				{
					printf("[+] Some gamedata struct = 0x%p\n", unkn_structure);
					while (!console_app_handler::m_on_exit_event)
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
}

void programm_routine()
{
	if (!initialize())
	{
		system("pause");
		utilites::shutdown_process();
	}

	loop();

	deinitialize();

	printf("[+] Exit...\n");

	Sleep(1000);
}
