#pragma once
#include <fmod_studio.hpp>

#include "SpeakerSettings.h"
#include "based/input/basedinput.h"
#include "based/managers/uimanager.h"

struct DialogueSet
{
public:
	DialogueSet() = default;
	DialogueSet(const std::string& path);

	std::string PopNextLine();

private:
	std::queue<std::string> mLines;
};

// TODO: Add a callback for when the dialogue finishes
class DialogueSystem
{
public:
	DialogueSystem() = default;
	~DialogueSystem() = default;

	void Initialize();
	void Update(float deltaTime);

	void SetCurrentDialogue(const std::string& path);
	void CloseCurrentDialogue();

	void SetTextSpeed(float speed) { mTextSpeed = speed; }

	entt::delegate<void()> mOnDialogueFinished;

	bool mInDialogue = false;
private:
	void ShowNextLine();
	void PlayCharacterSound(SpeakerSettings& speaker, char character);
	void OnNextPressed(const based::input::InputAction& action);

	based::managers::DocumentInfo* mDocument = nullptr;
	DialogueSet mCurrentDialogue;
	std::string mCurrentLine;
	std::string mShownLine;
	Rml::DataModelHandle mDataModelHandle;
	size_t mProgress;
	float mLastUpdate;
	float mTextSpeed = 0.04f;
	bool mIsTyping = false;
	int mCurrentSpeaker = 0;

	FMOD::Studio::EventInstance* mDialogueSounds;
	std::vector<SpeakerSettings> mSpeakers;
};