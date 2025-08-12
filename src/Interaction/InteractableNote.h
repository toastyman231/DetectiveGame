#pragma once
#include <fmod_studio.hpp>

#include "IInteractable.h"
#include "based/engine.h"

struct InteractableNote
{
	InteractableNote() = default;
	InteractableNote(const std::string& text) : mNoteText(text) {}
	InteractableNote(const std::filesystem::path& path)
	{
		std::ifstream ifs(path);
		std::stringstream sstream;
		sstream << ifs.rdbuf();
		ifs.close();
		mNoteText = sstream.str();
	}

	std::string mNoteText = "This is the default note text!";
	bool mIsOpen = false;
};

class InteractableNoteSystem : public IInteractable, Rml::EventListener
{
public:
	~InteractableNoteSystem() override {}

	void OnInteractionHoverEnter(Tool* tool) override;
	void OnInteractionHoverExit(Tool* tool) override;
	void OnInteract(Tool* tool) override;

	void Initialize();
	void Update(float deltaTime);
	void ProcessEvent(Rml::Event& event) override;

	FMOD::Studio::EventInstance* GetPageTurnEvent() const { return mPageTurnEvent; }

private:
	void OnInput(const based::input::InputAction& action);

	InteractableNote* mCurrentNote = nullptr;
	based::managers::DocumentInfo* mDocument = nullptr;
	Rml::DataModelHandle mDataModel;

	FMOD::Studio::EventInstance* mPageTurnEvent;
};