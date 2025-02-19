#pragma once

class PlayerControllerSystem
{
public:
	PlayerControllerSystem() = default;

	void Update(float deltaTime);

	float Speed = 6.0f;
	float JumpForce = 4.0f;
	float Sensitivity = 0.8f;

	bool AllowAirControl = false;

private:
	JPH::Vec3 mDesiredDirection;
	float mPitch = 0.f;
	float mYaw = 0.f;
};
