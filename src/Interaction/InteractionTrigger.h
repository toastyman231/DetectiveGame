#pragma once

struct InteractionTrigger
{
	InteractionTrigger() = default;
	InteractionTrigger(Tool* tool, uint32_t id) : mTool(tool), mId(id) {}

	bool operator== (const InteractionTrigger& other) const
	{
		return mId == other.mId;
	}

	Tool* mTool;
	bool mHoverEntered = false;
	bool mShouldExit = false;
	uint32_t mId;
};