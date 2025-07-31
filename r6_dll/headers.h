#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <string>
#include <d3d11.h>
#include <vector>
#include <thread>
#include <algorithm>

#include "math.h"
//
//#define IMGUI_DEFINE_MATH_OPERATORS
//#include "imgui.h"
//#include "imgui_internal.h"
//#include "imgui_impl_win32.h"
//#include "imgui_impl_dx11.h"
//
//#include "memory.hpp"
//
#include "classes.h"
#include "cache.h"
//
//#include "overlay.hpp"

inline uintptr_t g_module = 0;