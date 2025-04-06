#include "based/pch.h"
#include "SoundTestSystem.h"

#include "FMODSystem.h"
#include "based/engine.h"
#include "based/input/joystick.h"
#include "based/input/keyboard.h"
#include "based/input/mouse.h"
#include "../GameSystems.h"

Rml::EventListener* SoundEventListenerInstancer::InstanceEventListener(const Rml::String& value, Rml::Element* element)
{
	return new SoundEventListener(value, mSoundTester);
}

void SoundEventListener::ProcessEvent(Rml::Event& event)
{
	if (mValue == "sound_test_submit")
	{
		BASED_TRACE("EVENT SUBMITTED");

		std::string eventName = "event:/" + event.GetParameter<std::string>("event-name", "Test");
		if (!FMODSystem::DoesEventExist(eventName))
		{
			BASED_ERROR("FMOD event {} does not exist! Did you build your banks?", eventName);
			return;
		}

		BASED_TRACE("Playing event: {}", eventName);
		mSoundTester->ResetPlayingEvent(eventName);
	}

	if (mValue == "param_submit")
	{
		BASED_TRACE("PARAM SUBMITTED");

		std::string paramName = event.GetParameter<std::string>("param-name", "");
		float paramValue = event.GetParameter<float>("param-value", 0.f);

		BASED_TRACE("Setting param {} to {}", paramName, paramValue);
		FMODSystem::SetEventParameter(mSoundTester->GetPlayingEvent(), paramName, paramValue);
	}
}

void SoundTestSystem::Initialize()
{
	auto& uiManager = based::Engine::Instance().GetUiManager();
	auto context = uiManager.GetContext("main");

	Rml::Factory::RegisterEventListenerInstancer(mEventListenerInstancer);
	mDocument = uiManager.LoadWindow("SoundTesting", context);
	mDocument->document->Hide();
}

void SoundTestSystem::Update(float deltaTime)
{
	if (based::input::Keyboard::KeyDown(BASED_INPUT_KEY_F1))
	{
		if (mDocument->document->IsVisible())
		{
			mDocument->document->Hide();
			GameSystems::SetPlayerMouseLookEnabled(true);
			GameSystems::SetPlayerMovementEnabled(true);
			based::input::Mouse::SetCursorVisible(false);
			based::input::Mouse::SetCursorMode(based::input::CursorMode::Confined);
		} else
		{
			mDocument->document->Show();
			GameSystems::SetPlayerMouseLookEnabled(false);
			GameSystems::SetPlayerMovementEnabled(false);
			based::input::Mouse::SetCursorVisible(true);
			based::input::Mouse::SetCursorMode(based::input::CursorMode::Free);
		}
	}
}

void SoundTestSystem::ResetPlayingEvent(const std::string& event)
{
	if (mEventInstance) mEventInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE);

	mEventInstance = FMODSystem::CreateEvent(event);
	mEventInstance->start();
}

SoundTestSystem::SoundTestSystem()
{
	mEventListenerInstancer = new SoundEventListenerInstancer(this);
}

