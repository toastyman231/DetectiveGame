#pragma once
#include <string>
#include <unordered_map>

struct SpeakerSettings
{
	SpeakerSettings() = default;
	SpeakerSettings(float min, float max) : MinPitch(min), MaxPitch(max) {}

	std::string ConsonantEventPath = "event:/Dialogue/ConsonantSpecific";
	std::string VowelEventPath = "event:/Dialogue/VowelSpecific";
	std::string PunctuationEventPath = "event:/Dialogue/PunctuationSpecific";

	std::unordered_map<char, int> CharacterMap = {
		{'a', 0}, {'A', 0},
		{'e', 1}, {'E', 1},
		{'i', 2}, {'I', 2},
		{'o', 3}, {'O', 3},
		{'u', 4}, {'U', 4},

		{'b', 0}, {'B', 0},
		{'k', 1}, {'K', 1},
		{'d', 2}, {'D', 2},
		{'p', 3}, {'P', 3},
		{'j', 4}, {'J', 4},
		{'l', 5}, {'L', 5},
		{'m', 6}, {'M', 6},
		{'n', 7}, {'N', 7},
		{'t', 8}, {'T', 8},
		{'g', 9}, {'G', 9},

		{'!', 0},
		{'.', 1},
		{'?', 2},
	};

	float MinPitch = 1.f;
	float MaxPitch = 1.4f;
	int Frequency = 1;
};
