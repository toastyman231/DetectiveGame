#include "based/pch.h"
#include "AmbienceSystem.h"

#include "FMODSystem.h"
#include "based/app.h"
#include "based/engine.h"
#include "based/scene/entity.h"
#include "Jolt/Physics/Body/Body.h"

void AmbienceSystem::Initialize()
{
	if (!mAmbienceEvent)
	{
		mAmbienceEvent = FMODSystem::CreateFMODEvent("event:/LabAmbience");
		mAmbienceEvent->start();
	}
}

void AmbienceSystem::Shutdown()
{
	if (mAmbienceEvent)
	{
		mAmbienceEvent->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
		mAmbienceEvent->release();
	}
}

void AmbienceSystem::OnContactAdded(const JPH::CharacterVirtual* inCharacter, const JPH::BodyID& inBodyID2,
                                    const JPH::SubShapeID& inSubShapeID2, JPH::RVec3Arg inContactPosition, JPH::Vec3Arg inContactNormal,
                                    JPH::CharacterContactSettings& ioSettings)
{
	CharacterContactListener::OnContactAdded(inCharacter, inBodyID2, inSubShapeID2, inContactPosition, inContactNormal,
		ioSettings);

	auto& lockInterface = based::Engine::Instance().GetPhysicsManager().GetPhysicsSystem().GetBodyLockInterface();

	JPH::BodyLockRead lock(lockInterface, inBodyID2);
	if (lock.Succeeded())
	{
		auto& body = lock.GetBody();
		auto& registry = based::Engine::Instance().GetApp().GetCurrentScene()->GetRegistry();
		const entt::entity entity = static_cast<entt::entity>(body.GetUserData());

		if (registry.valid(entity) && registry.all_of<AmbienceArea>(entity))
		{
			auto& area = registry.get<AmbienceArea>(entity);
			mAmbienceEvent->setParameterByName("Location", area.mLocationID);
		}
	}
}
