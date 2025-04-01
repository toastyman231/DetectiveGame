#pragma once

#include <string>

#include "fmod.hpp"
#include "based/log.h"

class FMODSystem
{
public:
	static FMOD_RESULT InitializeFMOD()
	{
		auto result = FMOD::System_Create(&mFMODSystem);
		BASED_ASSERT(result == FMOD_OK, "Error creating FMOD system!");

		result = mFMODSystem->init(32, FMOD_INIT_NORMAL, nullptr);
		BASED_ASSERT(result == FMOD_OK, "Error initializing FMOD system!");

		return result;
	}

	static FMOD_RESULT PlaySound(const std::string& path, FMOD::Channel* channel = nullptr)
	{
		FMOD::Sound* sound;
		auto result = mFMODSystem->createSound(path.c_str(), FMOD_DEFAULT, nullptr, &sound);
		BASED_ASSERT(result == FMOD_OK, "Error creating FMOD sound!");

		result = mFMODSystem->playSound(sound, nullptr, false, &channel);
		BASED_ASSERT(result == FMOD_OK, "Error playing sound!");

		return result;
	}
private:
	inline static FMOD::System* mFMODSystem;
};