#pragma once
#include <codecvt>
#include <locale>
#include <algorithm>
#include "wiringRP.h"
#include "logo.h"
#include "devices/ks0108.h"
#include "devices/fonts/System5x7R.h"
#include "elevator_control.h"

std::wstring Utf8ToWchar(const char* utf8_str);
void PrintLines(std::vector<std::wstring>& lines);
std::vector<std::wstring> GetStrings(const std::wstring& str);
void PrintDisplayText(const std::wstring& str, long wait = 3);
void InitDisplay(elevator_control::Settings& settings);
void ReleaseWiringRP(int sig);