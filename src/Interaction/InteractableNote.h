#pragma once
#include "IInteractable.h"
#include "based/engine.h"

struct InteractableNote
{
	InteractableNote() = default;
	InteractableNote(const std::string& text) : mNoteText(text) {}

	~InteractableNote()
	{
		auto uiManager = based::Engine::Instance().GetUiManager();

		BASED_TRACE("Num docs on destroy: {}", uiManager.GetDocuments().size());
	}

	std::string mNoteText = "This is the default note text!";
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
	based::managers::DocumentInfo* mDocument = nullptr;
	Rml::DataModelHandle mDataModel;
};