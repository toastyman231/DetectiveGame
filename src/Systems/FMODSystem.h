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

		auto flags = FMOD_STUDIO_INIT_NORMAL;
#ifdef BASED_CONFIG_DEBUG
		flags = FMOD_STUDIO_INIT_LIVEUPDATE;
#endif
		result = mFMODSystem->initialize(32, flags, FMOD_DEFAULT, nullptr);
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

	static FMOD_RESULT LoadBanks(const std::string& rootPath)
	{
		auto dirs = std::queue<std::string>();
		dirs.push(rootPath);
		FMOD_RESULT result = FMOD_OK;

		uint32_t eventCount = 0;
		while (!dirs.empty())
		{
			auto currentDir = dirs.front();
			dirs.pop();

			for (const auto& dir : std::filesystem::directory_iterator(currentDir))
			{
				if (dir.is_directory())
				{
					dirs.push(dir.path().string());
					continue;
				}

				FMOD::Studio::Bank* bank;
				result = mFMODSystem->loadBankFile(dir.path().string().c_str(), 
					FMOD_STUDIO_LOAD_BANK_NORMAL, &bank);
				if (result != FMOD_OK)
				{
					BASED_ERROR("Error loading bank at {}", dir.path().string());
					continue;
				}

				int count;
				bank->getEventCount(&count);
				BASED_TRACE("Loaded FMOD bank {} with {} events", dir.path().string(), count);

				FMOD_GUID id;
				bank->getID(&id);
				mBanks[eventCount++] = bank;
			}
		}

		return result;
	}

	static FMOD::Studio::EventInstance* PlayEvent(const std::string& path)
	{
		FMOD::Studio::EventInstance* event;
		FMOD::Studio::EventDescription* eventDescription;
		auto result = mFMODSystem->getEvent(path.c_str(), &eventDescription);
		BASED_ASSERT(result == FMOD_OK, "Error loading event description!");

		result = eventDescription->createInstance(&event);
		BASED_ASSERT(result == FMOD_OK, "Error creating event instance!");

		result = event->start();
		if (result != FMOD_OK)
		{
			BASED_WARN("Could not play event {}!", path);
		}

		return event;
	}

	static FMOD::Studio::EventInstance* CreateEvent(const std::string& path)
	{
		FMOD::Studio::EventInstance* event;
		FMOD::Studio::EventDescription* eventDescription;
		auto result = mFMODSystem->getEvent(path.c_str(), &eventDescription);
		BASED_ASSERT(result == FMOD_OK, "Error loading event description!");

		result = eventDescription->createInstance(&event);
		BASED_ASSERT(result == FMOD_OK, "Error creating event instance!");

		return event;
	}

	static FMOD_RESULT Update(float deltaTime)
	{
		return mFMODSystem->update();
	}
private:
	inline static FMOD::Studio::System* mFMODSystem;
	inline static FMOD::System* mCoreSystem;

	inline static std::unordered_map<uint32_t, FMOD::Studio::Bank*> mBanks;
};