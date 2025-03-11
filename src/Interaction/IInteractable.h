#pragma once
#include "Tool.h"

struct Tool;

class IInteractable
{
public:
	virtual ~IInteractable() = default;

	virtual void OnInteractionHoverEnter(Tool* tool) = 0;
	virtual void OnInteractionHoverExit(Tool* tool) = 0;
	virtual void OnInteract(Tool* tool)
	{
		ToolSystem::CallOnInteract();
	}

	bool IsInteractable() const { return mInteractable; }

	void SetInteractionEnabled(bool enabled) { mInteractable = enabled; }
private:
	bool mInteractable = true;
};
