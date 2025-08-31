#include "based/pch.h"
#include "KeyImageBindings.h"

std::unordered_map<std::string, std::string> kbmMap = {
	{"", ""},
	{"E", "E_Key_Dark"},
	{"Escape", "Esc_Key_Dark"},
};

std::unordered_map<std::string, std::string> xboxMap = {
	{"", ""},
	{"Gamepad_FaceButton_Bottom", "XboxSeriesX_A"},
	{"Gamepad_FaceButton_Right", "XboxSeriesX_B"}
};

std::unordered_map<std::string, std::string> psMap = {
	{"", ""},
	{"Gamepad_FaceButton_Bottom", "PS5_Cross"},
	{"Gamepad_FaceButton_Right", "PS5_Circle"},
};

std::unordered_map<std::string, std::string> switchMap = {
	{"", ""},
	{"Gamepad_FaceButton_Bottom", "Switch_A"},
	{"Gamepad_FaceButton_Right", "Switch_B"},
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