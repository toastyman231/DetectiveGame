#pragma once

#include "based/scene/scene.h"

class MainScene : public based::scene::Scene
{
public:
	MainScene() : Scene("Main Scene") {}
	~MainScene() override = default;

	void InitializeScene() override;
	void UpdateScene(float deltaTime) override;
};