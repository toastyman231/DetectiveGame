#pragma once
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

private:
	void ShowNextLine();
	void CopyText(const std::string& source, std::string& destination, Rml::Element* elem);

	based::managers::DocumentInfo* mDocument = nullptr;
	DialogueSet mCurrentDialogue;
	std::string mCurrentLine;
	size_t mProgress;
	float mLastUpdate;
	float mTextSpeed = 0.04f;
	bool mIsTyping = false;
};