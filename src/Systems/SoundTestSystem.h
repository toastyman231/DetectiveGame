#pragma once
#include <fmod_studio.hpp>
#include <RmlUi/Core/EventListener.h>

#include "../UISystems/SolutionPanelSystem.h"

class SoundTestSystem;

class SoundEventListener : public Rml::EventListener
{
public:
	SoundEventListener(const std::string& value, SoundTestSystem* soundTester)
		: mValue(value), mSoundTester(soundTester) {}
	void ProcessEvent(Rml::Event& event) override;

private:
	std::string mValue;
	SoundTestSystem* mSoundTester;
};

class SoundEventListenerInstancer : public Rml::EventListenerInstancer
{
public:
	SoundEventListenerInstancer(SoundTestSystem* soundTester) : mSoundTester(soundTester) {}
	Rml::EventListener* InstanceEventListener(const Rml::String& value, Rml::Element* element) override;

private:
	SoundTestSystem* mSoundTester;
};

class SoundTestSystem 
{
public:
	SoundTestSystem();

	void Initialize();
	void Update(float deltaTime);

	void ResetPlayingEvent(const std::string& event);
	FMOD::Studio::EventInstance* GetPlayingEvent() const { return mEventInstance; }
private:
	based::managers::DocumentInfo* mDocument = nullptr;
	SoundEventListenerInstancer* mEventListenerInstancer = nullptr;
	FMOD::Studio::EventInstance* mEventInstance = nullptr;
};
