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

private:
	void ShowNextLine();

	based::managers::DocumentInfo* mDocument = nullptr;
	DialogueSet mCurrentDialogue;
};