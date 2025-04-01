#pragma once

#include <string>

#include "fmod.hpp"
#include "fmod_studio.hpp"
#include "based/log.h"

class FMODSystem
{
public:
	static FMOD_RESULT InitializeFMOD()
	{
		auto result = FMOD::Studio::System::create(&mFMODSystem);
		BASED_ASSERT(result == FMOD_OK, "Error creating FMOD system!");

		result = mFMODSystem->initialize(32, FMOD_STUDIO_INIT_NORMAL, FMOD_DEFAULT, nullptr);
		BASED_ASSERT(result == FMOD_OK, "Error initializing FMOD system!");

		result = mFMODSystem->getCoreSystem(&mCoreSystem);
		BASED_ASSERT(result == FMOD_OK, "Error initializing core system!");

		return result;
	}

	static FMOD_RESULT PlaySound(const std::string& path, FMOD::Channel* channel = nullptr)
	{
		FMOD::Sound* sound;
		auto result = mCoreSystem->createSound(path.c_str(), FMOD_DEFAULT, nullptr, &sound);
		BASED_ASSERT(result == FMOD_OK, "Error creating FMOD sound!");

		result = mCoreSystem->playSound(sound, nullptr, false, &channel);
		BASED_ASSERT(result == FMOD_OK, "Error playing sound!");

		return result;
	}

	static FMOD_RESULT Update(float deltaTime)
	{
		return mFMODSystem->update();
	}
private:
	inline static FMOD::Studio::System* mFMODSystem;
	inline static FMOD::System* mCoreSystem;
};