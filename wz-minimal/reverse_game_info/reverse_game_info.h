

//game upd 1.39.1.9739917

/*

na vsyakiy sluchai
Address of signature = ModernWarfare.exe + 0x0369AD0B
"\xE9\x00\x00\x00\x00\x4C\x8B\x00\x00\x4C\x8B\x00\x00\x00\x4C\x8B\x00\x00\x00\x48\x8B\x00\x00\x00\x48\x8B\x00\x00\x00\x4C\x8B\x00\x00\x00\x41\x8B\x00\x41\x89\x00\x00\x00\x00\x00\x41\x8B\x00\x00\x41\x89\x00\x00\x00\x00\x00\x41\x8B\x00\x00\x41\x89\x00\x00\x00\x00\x00\x48\x83\xC4", "x????xx??xx???xx???xx???xx???xx???xx?xx?????xx??xx?????xx??xx?????xxx"
"E9 ? ? ? ? 4C 8B ? ? 4C 8B ? ? ? 4C 8B ? ? ? 48 8B ? ? ? 48 8B ? ? ? 4C 8B ? ? ? 41 8B ? 41 89 ? ? ? ? ? 41 8B ? ? 41 89 ? ? ? ? ? 41 8B ? ? 41 89 ? ? ? ? ? 48 83 C4"
Address of signature = ModernWarfare.exe + 0x0351B1D0
"\xE9\x00\x00\x00\x00\x48\x8B\x00\x00\x4C\x8B\x00\x00\x00\x4C\x8B\x00\x00\x00\x4C\x8B\x00\x00\x00\x48\x8B\x00\x00\x00\x48\x8B\x00\x00\x00\x00\x00\x00\x4C\x8B\x00\x00\x00\x8B\x06\x41\x89\x00\x00\x00\x00\x00\x8B\x46\x00\x41\x89\x00\x00\x00\x00\x00\x8B\x46\x00\x41\x89\x00\x00\x00\x00\x00\x48\x81\xC4", "x????xx??xx???xx???xx???xx???xx??????xx???xxxx?????xx?xx?????xx?xx?????xxx"
"E9 ? ? ? ? 48 8B ? ? 4C 8B ? ? ? 4C 8B ? ? ? 4C 8B ? ? ? 48 8B ? ? ? 48 8B ? ? ? ? ? ? 4C 8B ? ? ? 8B 06 41 89 ? ? ? ? ? 8B 46 ? 41 89 ? ? ? ? ? 8B 46 ? 41 89 ? ? ? ? ? 48 81 C4"
"E9 ? ? ? ? ? 8B ? ? ? 8B ? ? ? ? 8B ? ? ? ? 8B
+= 0x25 to recoil x

Address of signature = ModernWarfare.exe + 0x0351B1CD
"\x4C\x03\x00\xE9\x00\x00\x00\x00\x48\x8B\x00\x00\x4C\x8B\x00\x00\x00\x4C\x8B\x00\x00\x00\x4C\x8B\x00\x00\x00\x48\x8B\x00\x00\x00\x48\x8B\x00\x00\x00\x00\x00\x00\x4C\x8B\x00\x00\x00\x8B\x06\x41\x89\x00\x00\x00\x00\x00\x8B\x46\x00\x41\x89\x00\x00\x00\x00\x00\x8B\x46\x00\x41\x89\x00\x00\x00\x00\x00\x48\x81\xC4", "xx?x????xx??xx???xx???xx???xx???xx??????xx???xxxx?????xx?xx?????xx?xx?????xxx"
"4C 03 ? E9 ? ? ? ? ? 8B ? ? ? 8B ? ? ? ? 8B ? ? ? ? 8B"


na vsyakiy sluchai
Address of signature = ModernWarfare.exe + 0x0369AD2D
"\x41\x8B\x00\x41\x89\x00\x00\x00\x00\x00\x41\x8B\x00\x00\x41\x89\x00\x00\x00\x00\x00\x41\x8B\x00\x00\x41\x89\x00\x00\x00\x00\x00\x48\x83\xC4", "xx?xx?????xx??xx?????xx??xx?????xxx"
"41 8B ? 41 89 ? ? ? ? ? 41 8B ? ? 41 89 ? ? ? ? ? 41 8B ? ? 41 89 ? ? ? ? ? 48 83 C4"
+= 0x3 to recoil x

na vsyakiy sluchai
Address of signature = ModernWarfare.exe + 0x0369AD10
"\x4C\x8B\x00\x00\x4C\x8B\x00\x00\x00\x4C\x8B\x00\x00\x00\x48\x8B\x00\x00\x00\x48\x8B\x00\x00\x00\x4C\x8B\x00\x00\x00\x41\x8B\x00\x41\x89\x00\x00\x00\x00\x00\x41\x8B\x00\x00\x41\x89\x00\x00\x00\x00\x00\x41\x8B\x00\x00\x41\x89\x00\x00\x00\x00\x00\x48\x83\xC4", "xx??xx???xx???xx???xx???xx???xx?xx?????xx??xx?????xx??xx?????xxx"
"4C 8B ? ? 4C 8B ? ? ? 4C 8B ? ? ? 48 8B ? ? ? 48 8B ? ? ? 4C 8B ? ? ? 41 8B ? 41 89 ? ? ? ? ? 41 8B ? ? 41 89 ? ? ? ? ? 41 8B ? ? 41 89 ? ? ? ? ? 48 83 C4"

Address of signature = ModernWarfare.exe + 0x0351B210
"\x41\x89\x00\x00\x00\x00\x00\x48\x81\xC4\x00\x00\x00\x00\x5E\x5D\xC3\x0F\x1F", "xx?????xxx????xxxxx"
"41 89 ? ? ? ? ? 48 81 C4 ? ? ? ? 5E 5D C3 0F 1F"

//recoil instruction x
	ModernWarfare.exe+371F666 - 41 89 80 EC880000     - mov [r8+000088EC],eax

	"\x41\x89\x00\x00\x00\x00\x00\x8B\x46\x00\x41\x89\x00\x00\x00\x00\x00\x8B\x46\x00\x41\x89\x00\x00\x00\x00\x00\x48\x83\xC4", "xx?????xx?xx?????xx?xx?????xxx"
	"41 89 ? ? ? ? ? 8B 46 ? 41 89 ? ? ? ? ? 8B 46 ? 41 89 ? ? ? ? ? 48 83 C4"


	ModernWarfare.exe+369AD30 - 41 89 80 D4880000     - mov [r8+000088D4],eax

	"\x41\x89\x00\x00\x00\x00\x00\x41\x8B\x00\x00\x41\x89\x00\x00\x00\x00\x00\x41\x8B\x00\x00\x41\x89\x00\x00\x00\x00\x00\x48\x83\xC4", "xx?????xx??xx?????xx??xx?????xxx"
	"41 89 ? ? ? ? ? 41 8B ? ? 41 89 ? ? ? ? ? 41 8B ? ? 41 89 ? ? ? ? ? 48 83 C4"


	ModernWarfare.exe+351B1FC - 41 89 80 A4FB0100     - mov [r8+0001FBA4],eax

	Address of signature = ModernWarfare.exe + 0x0351B1FC
	"\x41\x89\x00\x00\x00\x00\x00\x8B\x46\x00\x41\x89\x00\x00\x00\x00\x00\x8B\x46\x00\x41\x89\x00\x00\x00\x00\x00\x48\x81\xC4", "xx?????xx?xx?????xx?xx?????xxx"
	"41 89 ? ? ? ? ? 8B 46 ? 41 89 ? ? ? ? ? 8B 46 ? 41 89 ? ? ? ? ? 48 81 C4"


	8B 06 41 89 80 += 0x2
	"\x8B\x06\x41\x89\x80", "xxxxx"

	//function
	Address of signature = ModernWarfare.exe + 0x035163B0
	"\x48\x89\x00\x00\x00\x55\x56\x48\x8D\x00\x00\x00\x48\x81\xEC\x00\x00\x00\x00\x48\x63\x00\x48\x8B\x00\x48\x8D\x00\x00\x00\x00\x00\x4C\x8B\x00\x00\x00\x00\x00\x00\xC6\x45\x67\x00\x0F\xB6\x00\x00\x83\xC0\x00\x0F\xB6\x00\x65\x4C\x00\x00\x49\xF7\x00\x4D\x85\x00\x0F\x84\x00\x00\x00\x00\x48\x89\x00\x00\x00\x00\x00\x00\x49\x8B\x00\x48\x89\x00\x00\x00\x4C\x89\x00\x00\x00\x48\xC1\xC8\x00\x4C\x89\x00\x00\x00\x83\xE0\x00\x4C\x89\x00\x00\x00\x4C\x89\x00\x00\x00\x48\x83\xF8\x00\x0F\x87\x00\x00\x00\x00\x8B\x84\x00\x00\x00\x00\x00\x48\x03\x00\xFF\xE0\x48\xBA\x00\x00\x00\x00\x00\x00\x00\x00\x49\xB9\x00\x00\x00\x00\x00\x00\x00\x00\x9C\x51", "xx???xxxx???xxx????xx?xx?xx?????xx??????xxx?xx??xx?xx?xx??xx?xx?xx????xx??????xx?xx???xx???xxx?xx???xx?xx???xx???xxx?xx????xx?????xx?xxxx????????xx????????xx"
	"48 89 ? ? ? 55 56 48 8D ? ? ? 48 81 EC ? ? ? ? 48 63 ? 48 8B ? 48 8D ? ? ? ? ? 4C 8B ? ? ? ? ? ? C6 45 67 ? 0F B6 ? ? 83 C0 ? 0F B6 ? 65 4C ? ? 49 F7 ? 4D 85 ? 0F 84 ? ? ? ? 48 89 ? ? ? ? ? ? 49 8B ? 48 89 ? ? ? 4C 89 ? ? ? 48 C1 C8 ? 4C 89 ? ? ? 83 E0 ? 4C 89 ? ? ? 4C 89 ? ? ? 48 83 F8 ? 0F 87 ? ? ? ? 8B 84 ? ? ? ? ? 48 03 ? FF E0 48 BA ? ? ? ? ? ? ? ? 49 B9 ? ? ? ? ? ? ? ? 9C 51"

	caller

	Address of signature = ModernWarfare.exe + 0x045687C0
	"\x48\x89\x00\x00\x00\x48\x89\x00\x00\x00\x48\x89\x00\x00\x00\x57\x41\x00\x41\x00\x48\x83\xEC\x00\x48\x63\x00\x4C\x8D\x00\x00\x00\x00\x00\x44\x8B\x00\x49\x8B", "xx???xx???xx???xx?x?xxx?xx?xx?????xx?xx"
	"48 89 ? ? ? 48 89 ? ? ? 48 89 ? ? ? 57 41 ? 41 ? 48 83 EC ? 48 63 ? 4C 8D ? ? ? ? ? 44 8B ? 49 8B"
	//7FF7467088CA
	Address of signature = ModernWarfare.exe + 0x045688C5
	"\xE8\x00\x00\x00\x00\x48\x8D\x00\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x44\x8B\x00\x00\x00\x00\x00\x48\x8D\x00\x00\x00\x00\x00\x4C\x8D", "x????xx?????x????xx?????xx?????xx"
	"E8 ? ? ? ? 48 8D ? ? ? ? ? E8 ? ? ? ? 44 8B ? ? ? ? ? 48 8D ? ? ? ? ? 4C 8D"
	or
	Address of signature = ModernWarfare.exe + 0x045688CA
	"\x48\x8D\x00\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x44\x8B\x00\x00\x00\x00\x00\x48\x8D\x00\x00\x00\x00\x00\x4C\x8D", "xx?????x????xx?????xx?????xx"
	"48 8D ? ? ? ? ? E8 ? ? ? ? 44 8B ? ? ? ? ? 48 8D ? ? ? ? ? 4C 8D"



	Address of signature = ModernWarfare.exe + 0x035A871B
	"\xF3\x0F\x00\x00\x00\x00\x00\x00\x00\x41\xB8\x00\x00\x00\x00\xF3\x0F\x00\x00\x0F\x28", "xx???????xx????xx??xx"
	"F3 0F ? ? ? ? ? ? ? 41 B8 ? ? ? ? F3 0F ? ? 0F 28"

//recoil instruction y
	ModernWarfare.exe+371F670 - 41 89 80 F0880000     - mov [r8+000088F0],eax

	"\x41\x89\x00\x00\x00\x00\x00\x8B\x46\x00\x41\x89\x00\x00\x00\x00\x00\x48\x83\xC4", "xx?????xx?xx?????xxx"
	"41 89 ? ? ? ? ? 8B 46 ? 41 89 ? ? ? ? ? 48 83 C4"


	ModernWarfare.exe+369AD3B - 41 89 80 D8880000     - mov [r8+000088D8],eax

	Address of signature = ModernWarfare.exe + 0x0369AD3B
	"\x41\x89\x00\x00\x00\x00\x00\x41\x8B\x00\x00\x41\x89\x00\x00\x00\x00\x00\x48\x83\xC4", "xx?????xx??xx?????xxx"
	"41 89 ? ? ? ? ? 41 8B ? ? 41 89 ? ? ? ? ? 48 83 C4"


	ModernWarfare.exe+351B206 - 41 89 80 A8FB0100     - mov [r8+0001FBA8],eax

	"\x41\x89\x00\x00\x00\x00\x00\x8B\x46\x00\x41\x89\x00\x00\x00\x00\x00\x48\x81\xC4", "xx?????xx?xx?????xxx"
	"41 89 ? ? ? ? ? 8B 46 ? 41 89 ? ? ? ? ? 48 81 C4"



	Address of signature = ModernWarfare.exe + 0x035A871B
	"\xF3\x0F\x00\x00\x00\x00\x00\x00\x00\x41\xB8\x00\x00\x00\x00\xF3\x0F\x00\x00\x0F\x28", "xx???????xx????xx??xx"
	"F3 0F ? ? ? ? ? ? ? 41 B8 ? ? ? ? F3 0F ? ? 0F 28"
*/

/*
//scoped breath instruction x
	F3 44 0F11 08         - movss [rax],xmm9

	//sig for next updates
	"\xF3\x44\x00\x00\x00\xF3\x44\x00\x00\x00\x00\xF3\x44\x00\x00\x00\x00\x48\x8B\x00\x00\x00\x00\x00\x48\x33", "xx???xx????xx????xx?????xx"
	"F3 44 ? ? ? F3 44 ? ? ? ? F3 44 ? ? ? ? 48 8B ? ? ? ? ? 48 33"

//scoped breath instruction y
	F3 44 0F11 50 04      - movss [rax+04],xmm10

	//sig for next updates
	"\xF3\x44\x00\x00\x00\x00\xF3\x44\x00\x00\x00\x00\x48\x8B\x00\x00\x00\x00\x00\x48\x33", "xx????xx????xx?????xx"
	"F3 44 ? ? ? ? F3 44 ? ? ? ? 48 8B ? ? ? ? ? 48 33"
*/

/*
* //code intergity check
*
Address of signature = ModernWarfare.exe + 0x054931EF
"\xC6\x83\xC8\x00\x00\x00\x00\x0F\x95", "xxxxxx?xx"
"C6 83 C8 00 00 00 ? 0F 95"

//ptr ref mojno spizdit' is gui active
Address of signature = ModernWarfare.exe + 0x05495CB0
"\x48\x89\x00\x00\x00\x57\x48\x83\xEC\x00\x48\x8B\x00\x4C\x8D\x00\x00\x00\x00\x00\x48\x8B\x00\xBA\x00\x00\x00\x00\x48\x8B\x00\xE8\x00\x00\x00\x00\x4C\x8D", "xx???xxxx?xx?xx?????xx?x????xx?x????xx"
"48 89 ? ? ? 57 48 83 EC ? 48 8B ? 4C 8D ? ? ? ? ? 48 8B ? BA ? ? ? ? 48 8B ? E8 ? ? ? ? 4C 8D"

Address of signature = ModernWarfare.exe + 0x05493537
"\xC6\x83\xC8\x00\x00\x00\x00\xEB\x00\x48\x83\xBB\xB8\x00\x00\x00", "xxxxxx?x?xxxxxxx"
"C6 83 C8 00 00 00 ? EB ? 48 83 BB B8 00 00 00"

AB AA 26 C3 AB AA 26 43 AA AA EC 43 AB AA 49 44 00 00 00 3F 00 00 00 3F 00 00 18 43 55 55 6D 43

AB AA 26 C3 AB AA 26 43 AA AA EC 43 AB AA 49 44 00 00 00 3F 00 00 00 3F 00 00 18 43 55 55 6D 43 00 00 18 43 55 55 6D 43 00 00 00 00 00 00 80 3F 00 00 00 00 00 00 00 00 00 00 80 3F 00 00 80 3F

AB AA 26 C3 AB AA 26 43 AA AA EC 43 AB AA 49 44 00 00 00 3F 00 00 00 3F 00 00 18 43 55 55 6D 43 00 00 18 43 55 55 6D 43 00 00 00 00 00 00 80 3F 00 00 00 00 00 00 00 00 00 00 80 3F 00 00 80 3F 00 00 80 3F 00 00 80 3F 00 00 00 00 00 00 00 00 01 00 00 00 00 00 00 00 18 A1 7C 00 F6 01 00 00 98 E3 7D 00 F6 01 00 00 58 00 7E 00 F6 01 00 00

AB AA 26 C3 AB AA 26 43 AA AA EC 43 AB AA 49 44 00 00 00 3F 00 00 00 3F 00 00 18 43 55 55 6D 43 00 00 18 43 55 55 6D 43 00 00 00 00 00 00 80 3F 00 00 00 00 00 00 00 00 00 00 80 3F 00 00 80 3F 00 00 80 3F 00 00 00 00 00 00 00 00 00 00 00 00 01 00 00 00 00 00 00 00 18 A1 7C 00 F6 01 00 00 98 E3 7D 00 F6 01 00 00 58 00 7E 00 F6 01 00 00 18 FF 7D 00 F6 01 00 00 18 FF 7D 00 F6 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01 01 54 C9 B7 43
*/

/*
* //spread
//vostonovlenie
Address of signature = ModernWarfare.exe + 0x029A3B13
"\xF3\x0F\x00\x00\x00\x00\x00\x00\x83\xBB\xA8\x07\x00\x00", "xx??????xxxxxx"
"F3 0F ? ? ? ? ? ? 83 BB A8 07 00 00"
//


//func prologue
Address of signature = ModernWarfare.exe + 0x029A3440
"\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x00\x00\x4C\x8B\x00\x44\x0F", "xxx????xx??xx?xx"
"48 81 EC ? ? ? ? 48 8B ? ? 4C 8B ? 44 0F"
+= 0x781 to pattern 1

pattern 1
Address of signature = ModernWarfare.exe + 0x029A3BC1
"\x44\x0F\x00\x00\x00\x00\x00\x00\x00\xF3\x0F\x00\x00\x00\x00\x00\x00\x49\x8B", "xx???????xx??????xx"
"44 0F ? ? ? ? ? ? ? F3 0F ? ? ? ? ? ? 49 8B"
ModernWarfare.exe+29A3BC1 - 44 0F28 AC 24 50010000  - movaps xmm13,[rsp+00000150]
or
pattern 2
Address of signature = ModernWarfare.exe + 0x029A3BCA
"\xF3\x0F\x00\x00\x00\x00\x00\x00\x49\x8B\x00\x00\x8B\xBB", "xx??????xx??xx"
"F3 0F ? ? ? ? ? ? 49 8B ? ? 8B BB"
ModernWarfare.exe+29A3BCA - F3 0F11 83 8C070000   - movss [rbx+0000078C],xmm0

esli pattern 2 -= 0x9
C7 83 8C 07 00 00 00 00 02 C3 90 90 90 90 90 90 90
//


//prochee spread
//function prologue
Address of signature = ModernWarfare.exe + 0x029C8050
"\x4C\x89\x00\x00\x00\x89\x54\x00\x00\x53\x55\x56\x57\x41\x00\x41\x00\x41\x00\x41\x00\x48\x81\xEC", "xx???xx??xxxxx?x?x?x?xxx"
"4C 89 ? ? ? 89 54 ? ? 53 55 56 57 41 ? 41 ? 41 ? 41 ? 48 81 EC"
+= 0xBCC to ptr 1

//esli nujno otkluchit' crosshair shake -> nop
ModernWarfare.exe+284186F - F3 0F11 86 8C070000   - movss [rsi+0000078C],xmm0
Address of signature = ModernWarfare.exe + 0x029C8C1C
"\xF3\x0F\x00\x00\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xF3\x0F\x00\x00\x00\x00\x00\x00\xF3\x0F\x00\x00\x0F\x28\x00\x00\x00\x00\x00\x00\xF3\x0F", "xx??????x????xx??????xx??xx??????xx"
"F3 0F ? ? ? ? ? ? E8 ? ? ? ? F3 0F ? ? ? ? ? ? F3 0F ? ? 0F 28 ? ? ? ? ? ? F3 0F"
or -> += 0x8
Address of signature = ModernWarfare.exe + 0x029C8C14
"\xF3\x0F\x00\x00\x00\x00\x00\x00\xF3\x0F\x00\x00\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xF3\x0F\x00\x00\x00\x00\x00\x00\xF3\x0F\x00\x00\x0F\x28", "xx??????xx??????x????xx??????xx??xx"
"F3 0F ? ? ? ? ? ? F3 0F ? ? ? ? ? ? E8 ? ? ? ? F3 0F ? ? ? ? ? ? F3 0F ? ? 0F 28"
//


//hz toje dlya spread
Address of signature = ModernWarfare.exe + 0x029C8C3D
"\xF3\x0F\x00\x00\x00\x00\x00\x00\x48\x8B\x00\x00\x00\x00\x00\x48\x8B\x00\xE8\x00\x00\x00\x00\x84\xC0\x74\x00\x4C\x8B", "xx??????xx?????xx?x????xxx?xx"
"F3 0F ? ? ? ? ? ? 48 8B ? ? ? ? ? 48 8B ? E8 ? ? ? ? 84 C0 74 ? 4C 8B"

Address of signature = ModernWarfare.exe + 0x029A3AF3
"\xF3\x0F\x00\x00\x00\x00\x00\x00\xEB\x00\x45\x0F\x00\x00\xF3\x0F\x00\x00\x00\x00\x00\x00\xF3\x45", "xx??????x?xx??xx??????xx"
"F3 0F ? ? ? ? ? ? EB ? 45 0F ? ? F3 0F ? ? ? ? ? ? F3 45"
*/

/*
//code inergity check
Address of signature = ModernWarfare.exe + 0x1C182E92
"\x8B\x00\x33\x45\x00\x89\x45\x00\x83\x45\x70\x00\x8B\x45\x00\x83\xF8\x00\x0F\x8D\x00\x00\x00\x00\xE9\x00\x00\x00\x00\x39\xDF", "xxxx?xx?xxx?xx?xx?xx????x????xx"
"8B 00 33 45 ? 89 45 ? 83 45 70 ? 8B 45 ? 83 F8 ? 0F 8D ? ? ? ? E9 ? ? ? ? 39 DF"

Address of signature = ModernWarfare.exe + 0x1D9836D1
"\x8B\x00\x89\x45\x00\x48\x8D\x00\x00\x00\x48\x89\x00\x00\x8B\x4D\x00\xD3\x45\x00\x48\x8B\x00\x00\x48\x8D\x00\x00\x00\x83\x45\x78\x00\x8B\x45\x00\x03\x45\x00\x89\x45\x00\x8B\x45\x00\x33\x45\x00\x83\xE0\x00\x89\x45\x00\x48\x83\x45\x50\x00\x8B\x45\x00\x83\xF8\x00\x48\x89\x00\x00\x00\x48\x8D\x00\x00\x00\x48\xBD\x00\x00\x00\x00\x00\x00\x00\x00\x48\x87\x00\x00\x48\x89\x00\x00\x00\x48\x8D\x00\x00\x00\x48\x89\x00\x00\x00\x48\x8D\x00\x00\x00\x48\x8B\x00\x00\x00\x48\xB9", "xxxx?xx???xx??xx?xx?xx??xx???xxx?xx?xx?xx?xx?xx?xx?xx?xxxx?xx?xx?xx???xx???xx????????xx??xx???xx???xx???xx???xx???xx"
"8B 00 89 45 ? 48 8D ? ? ? 48 89 ? ? 8B 4D ? D3 45 ? 48 8B ? ? 48 8D ? ? ? 83 45 78 ? 8B 45 ? 03 45 ? 89 45 ? 8B 45 ? 33 45 ? 83 E0 ? 89 45 ? 48 83 45 50 ? 8B 45 ? 83 F8 ? 48 89 ? ? ? 48 8D ? ? ? 48 BD ? ? ? ? ? ? ? ? 48 87 ? ? 48 89 ? ? ? 48 8D ? ? ? 48 89 ? ? ? 48 8D ? ? ? 48 8B ? ? ? 48 B9"
*/

/*
radar write all:
Address of signature = ModernWarfare.exe + 0x05DB9570
"\x0F\x29\x00\x00\x0F\x29\x00\x00\x0F\x10\x00\x00\x0F\x10\x00\x00\x00\x48\x81\xC1", "xx??xx??xx??xx???xxx"
"0F 29 ? ? 0F 29 ? ? 0F 10 ? ? 0F 10 ? ? ? 48 81 C1"

uav accsess:
Address of signature = ModernWarfare.exe + 0x044847D0
"80 BB ? ? ? ? 00 75 3A 8B 93 ? ? ? ? 85 D2"
ModernWarfare.exe+44847D0 - 80 BB 58050000 00     - cmp byte ptr [rbx+00000558],00

accsess draw angle:
Address of signature = ModernWarfare.exe + 0x044A1EF1
v1 "41 80 BE ? ? ? ? 00 74 31 45 84 ED"
ModernWarfare.exe+44A1EF1 - 41 80 BE 5A050000 00  - cmp byte ptr [r14+0000055A],00

upd:
v1 "80 BE ? ? ? ? 00 74 31 45 84 ED"
"\x80\xBE\x00\x00\x00\x00\x00\x74\x31\x45\x84\xED", "xx????xxxxxx"
ModernWarfare.exe+47469D9 - 80 BE C6060100 00     - cmp byte ptr [rsi+000106C6],00
or
Address of signature = ModernWarfare.exe + 0x047469E0
"\x74\x00\x45\x84\x00\x74\x00\x41\x0F\x00\x00\xEB\x00\x49\x8B", "x?xx?x?xx??x?xx"
"74 ? 45 84 ? 74 ? 41 0F ? ? EB ? 49 8B"

accsess radar draw enemy:
Address of signature = ModernWarfare.exe + 0x04472F53
my sig, bcz sig scanner popil govna: 
v1 "80 BF ? ? ? ? 02 75 44 8B BF"
"\x80\xBF\x00\x00\x00\x00\x02\x75\x44\x8B\xBF", "xx????xxxxx"
v2 "80 BF ? ? ? ? 02 75 44 8B BF ? ? ? ? "
ModernWarfare.exe+4472F53 - 80 BF 5B050000 02     - cmp byte ptr [rdi+0000055B],02
or
Address of signature = ModernWarfare.exe + 0x04718AB3
"\x75\x00\x8B\xBF\x00\x00\x00\x00\x8B\x53", "x?xx????xx"
"75 ? 8B BF ? ? ? ? 8B 53"

caller draw enemy
Address of signature = ModernWarfare.exe + 0x04473172
"\xE8\x00\x00\x00\x00\x48\x8B\x00\x48\x8B\x00\xE8\x00\x00\x00\x00\x88\x43", "x????xx?xx?x????xx"
"E8 ? ? ? ? 48 8B ? 48 8B ? E8 ? ? ? ? 88 43"

Address of signature = ModernWarfare.exe + 0x05DB95BC
"\x0F\x10\x00\x00\x00\x75\x00\x0F\x29\x00\x00\x49\x83\xE0", "xx???x?xx??xxx"
"0F 10 ? ? ? 75 ? 0F 29 ? ? 49 83 E0"
ModernWarfare.exe+5DB95BC - 0F10 4C 11 F0         - movups xmm1,[rcx+rdx-10]

//toka esli uav == 1
Address of signature = ModernWarfare.exe + 0x04484D9F
"\x0F\xB6\x00\x00\x00\x00\x00\x3C\x00\x75\x00\x8B\x8E", "xx?????x?x?xx"
"0F B6 ? ? ? ? ? 3C ? 75 ? 8B 8E"
*/