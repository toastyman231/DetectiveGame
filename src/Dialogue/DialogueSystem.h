#pragma once
#include <fmod_studio.hpp>

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

	based::managers::DocumentInfo* mDocument = nullptr;
	DialogueSet mCurrentDialogue;
	std::string mCurrentLine;
	size_t mProgress;
	float mLastUpdate;
	float mTextSpeed = 0.04f;
	bool mIsTyping = false;

	FMOD::Studio::EventInstance* mDialogueSounds;
	int mFrequencyLevel = 2;
};