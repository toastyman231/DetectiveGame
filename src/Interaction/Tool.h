#pragma once
#include "ToolType.h"

struct InteractionTrigger;
class IInteractable;

struct Tool
{
	Tool() = default;
	Tool(float range) : mInteractRange(range) {}

	ToolType GetToolType() const { return mToolType; }
	void SetToolType(const ToolType& type) { mToolType = type; }

	float GetInteractionRange() const { return mInteractRange; }
	void SetInteractionRange(float range) { mInteractRange = range; }

	InteractionTrigger* GetCurrentTrigger() const { return mTrigger; }
	void SetCurrentTrigger(InteractionTrigger* trigger) { mTrigger = trigger; }

private:
	float mInteractRange = 500.f;

	ToolType mToolType = ToolType::Default;
	InteractionTrigger* mTrigger = nullptr;
};

class ToolSystem
{
public:
	void Update(float deltaTime);
};
