#pragma once
#include "ToolType.h"
#include "based/input/basedinput.h"

struct Interactable;
class IInteractable;

struct Tool
{
	Tool() = default;
	Tool(float range) : mInteractRange(range) {}

	ToolType GetToolType() const { return mToolType; }
	void SetToolType(const ToolType& type) { mToolType = type; }

	float GetInteractionRange() const { return mInteractRange; }
	void SetInteractionRange(float range) { mInteractRange = range; }

	Interactable* GetCurrentInteractable() const { return mCurrentInteractable; }
	void SetCurrentInteractable(Interactable* interactable);

private:
	float mInteractRange = 500.f;

	ToolType mToolType = ToolType::Default;
	Interactable* mCurrentInteractable = nullptr;
};

class ToolSystem
{
public:
	void Initialize();
	void Update(float deltaTime);

	void SetLocked(bool locked) { mIsLocked = locked; }
	bool IsLocked() const { return mIsLocked; }

	static void OnInteract(const based::input::InputAction& action);

	static void ShowInteractionUI(bool show);
private:
	bool mIsLocked = false;
};
