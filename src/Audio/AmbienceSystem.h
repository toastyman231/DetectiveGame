#pragma once

#include <fmod_studio.hpp>
#include <Jolt/Jolt.h>

#include "Jolt/Physics/Character/CharacterVirtual.h"
#include "Jolt/Physics/Collision/ContactListener.h"

struct AmbienceArea
{
	AmbienceArea(float locationID) : mLocationID(locationID) {}
	~AmbienceArea() = default;

	float mLocationID = 0.f;
};

class AmbienceSystem : JPH::CharacterContactListener
{
public:
	AmbienceSystem() = default;
	~AmbienceSystem() override = default;

	void Initialize();
	void Shutdown();

	void OnContactAdded(const JPH::CharacterVirtual* inCharacter, const JPH::BodyID& inBodyID2,
		const JPH::SubShapeID& inSubShapeID2, JPH::RVec3Arg inContactPosition, JPH::Vec3Arg inContactNormal,
		JPH::CharacterContactSettings& ioSettings) override;

private:

	FMOD::Studio::EventInstance* mAmbienceEvent;
};