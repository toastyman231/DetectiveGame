#pragma once

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
	void Update(float deltaTime);
};