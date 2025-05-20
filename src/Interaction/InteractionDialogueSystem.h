#pragma once
#include "IInteractable.h"

// Used to trigger a dialogue on interaction,
// and to inject the dialogue file path into the
// dialogue system
struct InteractionDialogueTrigger
{
	InteractionDialogueTrigger(const std::string& path) : mPath(path) {}

	std::string mPath;
};

class InteractionDialogueSystem : public IInteractable
{
public:
	void OnInteractionHoverEnter(Tool* tool) override;
	void OnInteractionHoverExit(Tool* tool) override;
	void OnInteract(Tool* tool) override;

	void Update(float deltaTime);

private:
	void OnDialogueFinished();

	InteractionDialogueTrigger* mCurrentDialogue = nullptr;
	entt::entity mCurrentDialogueEntity = entt::null;
};