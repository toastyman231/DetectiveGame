#pragma once

struct InteractionTrigger
{
	InteractionTrigger() = default;
	InteractionTrigger(Tool* tool) : mTool(tool) {}

	Tool* mTool;
	bool mHoverEntered = false;
	bool mShouldExit = false;
};