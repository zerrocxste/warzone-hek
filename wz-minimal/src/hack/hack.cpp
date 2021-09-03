#include "../includes.h"

class c_patch_pattern
{
private:
	BYTE* m_ptr_original_pattern;
	BYTE* m_ptr_modified_pattern;
	size_t m_size;
public:
	c_patch_pattern(size_t size) : m_size(size) {
		this->m_ptr_original_pattern = new BYTE[this->m_size];
		this->m_ptr_modified_pattern = new BYTE[this->m_size];
	}

	~c_patch_pattern() {
		clear();
	}

	void clear()
	{
		delete[] this->m_ptr_original_pattern;
		delete[] this->m_ptr_modified_pattern;
	}

	size_t get_size()
	{
		return this->m_size;
	}

	void setup_original(void* block, int byte = 0)
	{
		if (!block)
		{
			memset(this->m_ptr_original_pattern, byte, this->m_size);
			return;
		}

		memcpy(this->m_ptr_original_pattern, block, this->m_size);
	}

	void setup_modified(void* block, int byte = 0)
	{
		if (!block)
		{
			memset(this->m_ptr_modified_pattern, byte, this->m_size);
			return;
		}

		memcpy(this->m_ptr_modified_pattern, block, this->m_size);
	}

	void* get_original()
	{
		return this->m_ptr_original_pattern;
	}

	void* get_modified()
	{
		return this->m_ptr_modified_pattern;
	}
};

struct basic_process_information
{
	HANDLE access_handle;
	DWORD pid;
	DWORD_PTR base_address;
	DWORD_PTR base_size;
	DWORD_PTR base_end;
	DWORD_PTR min_application_address_space;
	DWORD_PTR max_application_address_space;

	~basic_process_information()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct offsets
{
	DWORD_PTR prologue_encrypted_function;
	DWORD_PTR instance_game_state_struct;
	DWORD_PTR weapon_recoil_x_axis;
	DWORD_PTR weapon_recoil_y_axis;
	DWORD_PTR weapon_breath_x_axis;
	DWORD_PTR weapon_breath_y_axis;
	DWORD_PTR radar_draw_enemy;

	~offsets()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct memory_patches
{
	c_patch_pattern* recoil_axis_x = new c_patch_pattern(7);
	c_patch_pattern* recoil_axis_y = new c_patch_pattern(7);
	c_patch_pattern* breath_axis_x = new c_patch_pattern(5);
	c_patch_pattern* breath_axis_y = new c_patch_pattern(6);
	c_patch_pattern* radar = new c_patch_pattern(2);

	~memory_patches()
	{
		this->recoil_axis_x->clear();
		this->recoil_axis_y->clear();
		this->breath_axis_x->clear();
		this->breath_axis_y->clear();
		this->radar->clear();
	}
};

basic_process_information g_mw_process;
offsets g_offsets;
memory_patches g_memory_patches;

bool predefinition_game_process()
{
	auto process_name = "ModernWarfare.exe";

	auto process = utilites::get_process(process_name);

	if (!process)
	{
		printf("[-] Not found MW19 process\n");
		return false;
	}

	g_mw_process.pid = process->th32ProcessID;

	auto base = utilites::find_module(g_mw_process.pid, process_name);

	if (!base)
	{
		printf("[-] Not found MW19 module\n");
		return false;
	}

	g_mw_process.base_address = (DWORD_PTR)base->modBaseAddr;
	g_mw_process.base_size = (DWORD_PTR)base->modBaseSize;
	g_mw_process.base_end = g_mw_process.base_address + g_mw_process.base_size;

	printf("[+] Found MW19 process. pid = %d, base = 0x%p, base size = %I64X, base end = 0x%p\n",
		g_mw_process.pid,
		g_mw_process.base_address,
		g_mw_process.base_size,
		g_mw_process.base_end);

	g_mw_process.access_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, g_mw_process.pid);

	if (!g_mw_process.access_handle)
	{
		printf("[-] Failed open process\n");
		return false;
	}

	SYSTEM_INFO info{};
	GetSystemInfo(&info);

	g_mw_process.min_application_address_space = (DWORD_PTR)info.lpMinimumApplicationAddress;
	g_mw_process.max_application_address_space = (DWORD_PTR)info.lpMaximumApplicationAddress;

	return true;
}

bool save_original_bytes()
{
	if (!g_memory_patches.recoil_axis_x->get_original() ||
		!g_memory_patches.recoil_axis_y->get_original() ||
		!g_memory_patches.breath_axis_x->get_original() ||
		!g_memory_patches.breath_axis_y->get_original() ||
		!g_memory_patches.radar->get_original())
	{
		printf("[-] Failed allocate memory for save original bytes\n");
		return false;
	}

	auto _read_status1 = ReadProcessMemory(g_mw_process.access_handle, 
		(void*)g_offsets.weapon_recoil_x_axis, 
		g_memory_patches.recoil_axis_x->get_original(),
		g_memory_patches.recoil_axis_x->get_size(), 
		NULL);

	auto _read_status2 = ReadProcessMemory(g_mw_process.access_handle,
		(void*)g_offsets.weapon_recoil_y_axis, 
		g_memory_patches.recoil_axis_y->get_original(), 
		g_memory_patches.recoil_axis_y->get_size(), 
		NULL);
	auto _read_status3 = ReadProcessMemory(g_mw_process.access_handle, 
		(void*)g_offsets.weapon_breath_x_axis,
		g_memory_patches.breath_axis_x->get_original(),
		g_memory_patches.breath_axis_x->get_size(), 
		NULL);

	auto _read_status4 = ReadProcessMemory(g_mw_process.access_handle,
		(void*)g_offsets.weapon_breath_y_axis, 
		g_memory_patches.breath_axis_y->get_original(), 
		g_memory_patches.breath_axis_y->get_size(),
		NULL);

	auto _read_status5 = ReadProcessMemory(g_mw_process.access_handle, 
		(void*)g_offsets.radar_draw_enemy,
		g_memory_patches.radar->get_original(),
		g_memory_patches.radar->get_size(), 
		NULL);

	_read_status1 ? printf("[+] Recoil x axis instruction dumped\n") : printf("[-] Recoil x axis instruction dump failed\n");
	_read_status2 ? printf("[+] Recoil y axis instruction dumped\n") : printf("[-] Recoil y axis instruction dump failed\n");
	_read_status3 ? printf("[+] Breath x axis instruction dumped\n") : printf("[-] Breath x axis instruction dump failed\n");
	_read_status4 ? printf("[+] Breath y axis instruction dumped\n") : printf("[-] Breath y axis instruction dump failed\n");
	_read_status5 ? printf("[+] Radar enemy check instruction dumped\n") : printf("[-] Radar enemy check instruction dump failed\n");

	return _read_status1 && _read_status2 && _read_status3 && _read_status4 && _read_status5;
}

bool make_patches()
{
	if (!g_memory_patches.recoil_axis_x->get_modified() ||
		!g_memory_patches.recoil_axis_y->get_modified() ||
		!g_memory_patches.breath_axis_x->get_modified() ||
		!g_memory_patches.breath_axis_y->get_modified() ||
		!g_memory_patches.radar->get_modified())
	{
		printf("[-] Failed allocate memory for make patches\n");
		return false;
	}

	g_memory_patches.recoil_axis_x->setup_modified(NULL, 0x90);
	g_memory_patches.recoil_axis_y->setup_modified(NULL, 0x90);
	g_memory_patches.breath_axis_x->setup_modified(NULL, 0x90);
	g_memory_patches.breath_axis_y->setup_modified(NULL, 0x90);
	g_memory_patches.radar->setup_modified(NULL, 0x90);

	return true;
}

bool find_encrypted_function()
{
	auto e8xxxx_encrypted_func = utilites::pattern_scanner_ex(g_mw_process.access_handle,
		g_mw_process.base_address, g_mw_process.base_end,
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
	ReadProcessMemory(g_mw_process.access_handle, (void*)(e8xxxx_encrypted_func + 0x1), &rva_encrypted_function, sizeof(DWORD), NULL);

	printf("[+] Relative offset to encrypted function from E8 XXXX = 0x%X\n", rva_encrypted_function);

	g_offsets.prologue_encrypted_function = utilites::asm64_solve_dest(e8xxxx_encrypted_func + 0x5, rva_encrypted_function);

	printf("[+] Encrypted function found = 0x%p\n", g_offsets.prologue_encrypted_function);

	return true;
}

bool find_game_state_structure()
{
	//"48 8B ? ? ? ? ? BA ? ? ? ? 48 8B ? E8 ? ? ? ? 48 8B ? 48 85 ? 75 ? 33 DB EB ? 0F B7 ? B9 ? ? ? ? 66 3B ? 73"
	auto __mov_rbx_pofxxxx__va = utilites::pattern_scanner_ex(g_mw_process.access_handle, 
		g_mw_process.base_address, g_mw_process.base_end,
		"\x48\x8B\x00\x00\x00\x00\x00\xBA\x00\x00\x00\x00\x48\x8B\x00\xE8\x00\x00\x00\x00\x48\x8B\x00\x48\x85\x00\x75\x00\x33\xDB\xEB\x00\x0F\xB7\x00\xB9\x00\x00\x00\x00\x66\x3B\x00\x73",
		"xx?????x????xx?x????xx?xx?x?xxx?xx?x????xx?x",
		0x1, 
		PAGE_EXECUTE_READWRITE);

	if (!__mov_rbx_pofxxxx__va)
	{
		printf("[-] Not found required instruction\n");
		return false;
	}

	DWORD rva_instance_game_state_struct = 0;
	ReadProcessMemory(g_mw_process.access_handle, (void*)(__mov_rbx_pofxxxx__va + 0x3), &rva_instance_game_state_struct, sizeof(DWORD), NULL);

	ReadProcessMemory(g_mw_process.access_handle, (void*)utilites::asm64_solve_dest(__mov_rbx_pofxxxx__va + 0x7, rva_instance_game_state_struct), &g_offsets.instance_game_state_struct, sizeof(DWORD_PTR), NULL);

	if (!g_offsets.instance_game_state_struct)
	{
		printf("[-] Not found game state struct\n");
		return false;
	}

	printf("[+] Game state struct = 0x%p\n", g_offsets.instance_game_state_struct);

	return true;
}

bool find_va_for_hack_features()
{
	if (g_offsets.prologue_encrypted_function && 
		(g_offsets.weapon_recoil_x_axis = utilites::pattern_scanner_ex(g_mw_process.access_handle,
		g_offsets.prologue_encrypted_function, 
		g_mw_process.base_end,
		"\x8B\x06\x41\x89\x80", "xxxxx",
		0x1,
		PAGE_EXECUTE_READWRITE)))
	{
		g_offsets.weapon_recoil_x_axis += 0x2;

		g_offsets.weapon_recoil_y_axis = utilites::pattern_scanner_ex(g_mw_process.access_handle,
			g_offsets.weapon_recoil_x_axis + 0x1,
			g_offsets.weapon_recoil_x_axis + 0x50,
			"\x41\x89\x80", "xxx",
			0x1,
			PAGE_EXECUTE_READWRITE);
	}

	g_offsets.weapon_breath_x_axis = utilites::pattern_scanner_ex(g_mw_process.access_handle,
		g_mw_process.base_address,
		g_mw_process.base_end,
		"\xF3\x44\x00\x00\x00\xF3\x44\x00\x00\x00\x00\xF3\x44\x00\x00\x00\x00\x48\x8B\x00\x00\x00\x00\x00\x48\x33", "xx???xx????xx????xx?????xx",
		0x1,
		PAGE_EXECUTE_READWRITE);

	g_offsets.weapon_breath_y_axis = utilites::pattern_scanner_ex(g_mw_process.access_handle,
		g_mw_process.base_address,
		g_mw_process.base_end,
		"\xF3\x44\x00\x00\x00\x00\xF3\x44\x00\x00\x00\x00\x48\x8B\x00\x00\x00\x00\x00\x48\x33", "xx????xx????xx?????xx",
		0x1,
		PAGE_EXECUTE_READWRITE);

	if (g_offsets.radar_draw_enemy = utilites::pattern_scanner_ex(g_mw_process.access_handle,
		g_mw_process.base_address,
		g_mw_process.base_end,
		"\x80\xBF\x00\x00\x00\x00\x02\x75\x44\x8B\xBF", "xx????xxxxx",
		0x1,
		PAGE_EXECUTE_READWRITE))
	{
		g_offsets.radar_draw_enemy += 0x7;
	}
	else
	{
		printf("[-] First radar pattern is no longer valid, research for second pattern\n");
		g_offsets.radar_draw_enemy = utilites::pattern_scanner_ex(g_mw_process.access_handle,
			g_mw_process.base_address,
			g_mw_process.base_end,
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
		g_offsets.weapon_recoil_x_axis, g_offsets.weapon_recoil_x_axis - g_mw_process.base_address,
		g_offsets.weapon_recoil_y_axis, g_offsets.weapon_recoil_y_axis - g_mw_process.base_address,
		g_offsets.weapon_breath_x_axis, g_offsets.weapon_breath_x_axis - g_mw_process.base_address,
		g_offsets.weapon_breath_y_axis, g_offsets.weapon_breath_y_axis - g_mw_process.base_address,
		g_offsets.radar_draw_enemy, g_offsets.radar_draw_enemy - g_mw_process.base_address
	);

	return g_offsets.weapon_recoil_x_axis &&
		g_offsets.weapon_recoil_y_axis &&
		g_offsets.weapon_breath_x_axis &&
		g_offsets.weapon_breath_y_axis &&
		g_offsets.radar_draw_enemy;
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

DWORD_PTR find_some_structure_instance()
{
	return utilites::pattern_scanner_ex(g_mw_process.access_handle, g_mw_process.min_application_address_space, g_mw_process.max_application_address_space,
		"\xAB\xAA\x26\xC3\xAB\xAA\x26\x43\xAA\xAA\xEC\x43\xAB\xAA\x49\x44\x00\x00\x00\x3F\x00\x00\x00"
		"\x3F\x00\x00\x18\x43\x55\x55\x6D\x43\x00\x00\x18\x43\x55\x55\x6D\x43\x00\x00\x00\x00\x00\x00"
		"\x80\x3F\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x80\x3F\x00\x00\x80\x3F",
		"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		0x1,
		PAGE_READWRITE);
}

void enable_hacks()
{
	printf("\n[+] Enable hacks...\n");

	auto recoil_x_patch_status = WriteProcessMemory(g_mw_process.access_handle, 
		(void*)(g_offsets.weapon_recoil_x_axis), 
		g_memory_patches.recoil_axis_x->get_modified(), g_memory_patches.recoil_axis_x->get_size(), 
		NULL);

	auto recoil_y_patch_status = WriteProcessMemory(g_mw_process.access_handle, 
		(void*)(g_offsets.weapon_recoil_y_axis), 
		g_memory_patches.recoil_axis_y->get_modified(), g_memory_patches.recoil_axis_y->get_size(), 
		NULL);

	auto breath_x_patch_status = WriteProcessMemory(g_mw_process.access_handle, 
		(void*)(g_offsets.weapon_breath_x_axis), 
		g_memory_patches.breath_axis_x->get_modified(), g_memory_patches.breath_axis_x->get_size(), 
		NULL);

	auto breath_y_patch_status = WriteProcessMemory(g_mw_process.access_handle, 
		(void*)(g_offsets.weapon_breath_y_axis), 
		g_memory_patches.breath_axis_y->get_modified(), g_memory_patches.breath_axis_y->get_size(), 
		NULL);

	auto radar_patch_status = WriteProcessMemory(g_mw_process.access_handle, 
		(void*)(g_offsets.radar_draw_enemy), 
		g_memory_patches.radar->get_modified(), g_memory_patches.radar->get_size(),
		NULL);

	if (!recoil_x_patch_status
		|| !recoil_y_patch_status)
		printf("[-] Recoil patch something wrong\n");
	else
		printf("[+] No recoil enabled\n");

	if (!breath_x_patch_status
		|| !breath_y_patch_status)
		printf("[-] Breath patch something wrong\n");
	else
		printf("[+] No breath enabled\n");

	if (!radar_patch_status)
		printf("[-] Radar draw enemy patch something wrong\n");
	else
		printf("[+] Radar draw enemy enabled\n");
}

void restore_original_code()
{
	printf("\n[+] Disable hacks...\n");

	auto recoil_x_patch_status = WriteProcessMemory(g_mw_process.access_handle, 
		(void*)(g_offsets.weapon_recoil_x_axis), 
		g_memory_patches.recoil_axis_x->get_original(), g_memory_patches.recoil_axis_x->get_size(), 
		NULL);

	auto recoil_y_patch_status = WriteProcessMemory(g_mw_process.access_handle, 
		(void*)(g_offsets.weapon_recoil_y_axis), 
		g_memory_patches.recoil_axis_y->get_original(), g_memory_patches.recoil_axis_y->get_size(), 
		NULL);

	auto breath_x_patch_status = WriteProcessMemory(g_mw_process.access_handle, 
		(void*)(g_offsets.weapon_breath_x_axis), 
		g_memory_patches.breath_axis_x->get_original(), g_memory_patches.breath_axis_x->get_size(), 
		NULL);

	auto breath_y_patch_status = WriteProcessMemory(g_mw_process.access_handle, 
		(void*)(g_offsets.weapon_breath_y_axis), 
		g_memory_patches.breath_axis_y->get_original(), g_memory_patches.breath_axis_y->get_size(), 
		NULL);

	auto radar_patch_status = WriteProcessMemory(g_mw_process.access_handle, 
		(void*)(g_offsets.radar_draw_enemy), 
		g_memory_patches.radar->get_original(), g_memory_patches.radar->get_size(), 
		NULL);

	if (!recoil_x_patch_status
		|| !recoil_y_patch_status)
		printf("[-] Restore recoil code something wrong\n");
	else
		printf("[+] Recoil restored\n");

	if (!breath_x_patch_status
		|| !breath_y_patch_status)
		printf("[-] Restore breath code something wrong\n");
	else
		printf("[+] No breath restored\n");

	if (!radar_patch_status)
		printf("[-] Radar restore code something wrong\n");
	else
		printf("[+] Radar code restored\n");
}

void loop()
{
	bool is_enabled = false;
	bool is_key_pressed = false;
	DWORD sleep_timer = 0;
	while (!console_app_handler::m_on_exit_event)
	{
		bool in_game = false;
		if (ReadProcessMemory(g_mw_process.access_handle, (void*)(g_offsets.instance_game_state_struct + 0x238 /*0x988*/), &in_game, sizeof(bool), NULL)
			&& in_game)
		{
			if (GetTickCount() - sleep_timer >= 25000)
			{
				if (auto some_structure_instance = find_some_structure_instance())
				{
					printf("[+] Some gamedata struct instance = 0x%p\n", some_structure_instance);
					bool is_changed = false;
					bool maybe_integrity_check_active = false;
					int skip_frame = 0;
					while (!console_app_handler::m_on_exit_event)
					{
						bool hud_active = false;
						bool first_person_is_not_have_weapon = true;

						if (ReadProcessMemory(g_mw_process.access_handle, (void*)(g_offsets.instance_game_state_struct + 0x379), &first_person_is_not_have_weapon, sizeof(bool), NULL)
							&& ReadProcessMemory(g_mw_process.access_handle, (void*)(some_structure_instance + 0xC8), &hud_active, sizeof(bool), NULL)
							&& (!first_person_is_not_have_weapon && hud_active))
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
							skip_frame = 0;
						}
						else
						{
							skip_frame++;
							if (maybe_integrity_check_active && is_enabled && skip_frame > 1)
							{
								restore_original_code();
								printf("[!] Restore code from cod integrity check (hacks disabled)\n");
								maybe_integrity_check_active = false;
							}
						}

						BYTE structure_status = 0;
						if (!ReadProcessMemory(g_mw_process.access_handle, (void*)some_structure_instance, &structure_status, sizeof(BYTE), NULL)
							|| structure_status != 0xAB)
						{
							if (maybe_integrity_check_active && is_enabled)
							{
								restore_original_code();
								maybe_integrity_check_active = false;
							}
							printf("[!] Structure is no longer valid, disabled hacks. Research will start when you are in the game\n");
							break;
						}

						Sleep(1);
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
}

void deinitialize()
{
	CloseHandle(g_mw_process.access_handle);
}

void hack::pornhub_invoke()
{
	if (!initialize())
	{
#ifdef DEBUG
		system("pause");
#endif
		return;
	}

	printf("\n[!] Use MOUSE4 for activate / deactivate hacks\n\n");

	loop();

	deinitialize();

	printf("[+] Exit...\n");

	Sleep(1000);
}
