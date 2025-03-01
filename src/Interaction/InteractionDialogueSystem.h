#pragma once
#include "IInteractable.h"

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
};