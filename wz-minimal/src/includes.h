#pragma once

#define _CRT_SECURE_NO_WARNINGS

#ifdef WIN32
#error "compile as x64 is required"
#endif

#pragma warning (disable: 6011)
#pragma warning (disable: 28159)

#define SHOW_CONSOLE
#define DEBUG
//#define ON_TOP

#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <psapi.h>

#include "console_app_handler/console_app_handler.h"
#include "utilites/utilites.h"
#include "hack/hack.h"

#ifndef DEBUG
#define printf(fmt, ...) (0)
#endif