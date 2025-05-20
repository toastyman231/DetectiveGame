#pragma once
#include "based/core/gameplaytags.h"

struct Interactable
{
	void OnHoverEnter()
	{
		tags.AddTag(based::core::Tag("Interaction.Hover"));
	}
	void OnHoverExit()
	{
		tags.AddTag(based::core::Tag("Interaction.Unhover"));
		tags.RemoveTags({based::core::Tag("Interaction.Interact"), based::core::Tag("Interaction.Hover") });
	}
	void OnInteract()
	{
		tags.AddTag(based::core::Tag("Interaction.Interact"));
	}

	based::core::TagContainer tags;
	Tool* tool = nullptr;
	bool mCanInteract = true;
};