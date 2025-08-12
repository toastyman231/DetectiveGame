#pragma once
#include "../GameSystems.h"
#include "../UISystems/SolutionPanelSystem.h"
#include "based/core/gameplaytags.h"

struct Interactable
{
	Interactable() = default;
	Interactable(const std::vector<SolutionWord>& solutionWords) : mSolutionWords(solutionWords) {}
	Interactable(const std::vector<SolutionWord>& solutionWords, FMOD::Studio::EventInstance* interactSound)
		: mSolutionWords(solutionWords)
		, mInteractSound(interactSound)
	{}

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

		for (auto& solutionWord : mSolutionWords)
		{
			GameSystems::mSolutionPanelSystem.AddWord(solutionWord);
		}

		if (mInteractSound)
		{
			mInteractSound->start();
		}
	}

	based::core::TagContainer tags;
	Tool* tool = nullptr;
	bool mCanInteract = true;
	std::vector<SolutionWord> mSolutionWords;

	FMOD::Studio::EventInstance* mInteractSound = nullptr;
};