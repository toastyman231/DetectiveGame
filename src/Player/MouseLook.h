#pragma once
#include "based/input/basedinput.h"

struct MouseLook
{
	MouseLook() = default;
	MouseLook(float sensitivity) : mSensitivity(sensitivity) {}

	float mPitch = 0.f;
	float mYaw = 0.f;
	float mSensitivity = 100.f;
	bool mCanLook = true;
};

class MouseLookSystem
{
public:
	void Initialize();
	void Update(float deltaTime);

private:
	void HandleInput(const based::input::InputAction& action);
};