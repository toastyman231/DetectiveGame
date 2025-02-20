#pragma once
#include "IInteractable.h"

struct InteractableNote
{
	InteractableNote() = default;
	InteractableNote(const std::string& text) : mNoteText(text) {}

	std::string mNoteText = "This is the default note text!";
	Rml::ElementDocument* mDocument = nullptr;
	bool mIsOpen = false;
};

class InteractableNoteSystem : public IInteractable
{
public:
	~InteractableNoteSystem() override {}

	void OnInteractionHoverEnter(Tool* tool) override;
	void OnInteractionHoverExit(Tool* tool) override;
	void OnInteract(Tool* tool) override;

	void Update(float deltaTime);

private:
	InteractableNote* mCurrentNote = nullptr;
};