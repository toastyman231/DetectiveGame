#include "based/pch.h"
#include "KeyImageBindings.h"

std::unordered_map<std::string, std::string> kbmMap = {
	{"", ""},
	{"E", "E_Key_Dark"},
	{"Escape", "Esc_Key_Dark"},
};

std::unordered_map<std::string, std::string> xboxMap = {
	{"", ""},
	{"a", "XboxSeriesX_A"},
	{"b", "XboxSeriesX_B"}
};

std::unordered_map<std::string, std::string> psMap = {
	{"", ""},
	{"a", "PS5_Cross"},
	{"b", "PS5_Circle"},
};

std::unordered_map<std::string, std::string> switchMap = {
	{"", ""},
	{"a", "Switch_A"},
	{"b", "Switch_B"},
};

std::unordered_map<std::string, std::unordered_map<std::string, std::string>> mKeyMaps = {
		{"KBM", kbmMap},
		{"Xbox360", xboxMap},
		{"XboxOne", xboxMap},
		{"PS3", psMap},
		{"PS4", psMap},
		{"PS5", psMap},
		{"SwitchPro", switchMap},
};