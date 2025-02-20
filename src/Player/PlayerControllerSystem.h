#pragma once

class PlayerControllerSystem
{
public:
	PlayerControllerSystem() = default;

	void Update(float deltaTime);

private:
	JPH::Vec3 mDesiredDirection;
};
