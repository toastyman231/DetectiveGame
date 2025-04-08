#pragma once
#include <fmod_studio.hpp>

class PlayerControllerSystem
{
public:
	PlayerControllerSystem() = default;

	void Initialize(float stepInterval = 0.4f);
	void Update(float deltaTime);

private:
	JPH::Vec3 mDesiredDirection;

	FMOD::Studio::EventInstance* mFootsteps0;
	FMOD::Studio::EventInstance* mFootsteps1;
	FMOD::Studio::EventInstance* mCreaks;

	float mStepInterval = 0.4f;
	float mCreakChance = 0.1f;
	float mLastStepTime = 0;
	int mStepIndex = 1;
};
