#include "based/pch.h"
#include "DialogueSystem.h"

#include <future>

#include "../GameSystems.h"
#include "../Systems/FMODSystem.h"
#include "based/app.h"
#include "based/engine.h"
#include "based/core/basedtime.h"
#include "based/input/mouse.h"
#include "based/math/random.h"
#include "based/scene/components.h"

DialogueSet::DialogueSet(const std::string& path)
{
	std::ifstream ifs(path);
	std::stringstream fileContent;

	fileContent << ifs.rdbuf();
	ifs.close();

	std::string line;
	while (std::getline(fileContent, line))
	{
		mLines.emplace(line);
	}
}

std::string DialogueSet::PopNextLine()
{
	if (mLines.empty()) return "";

	auto line = mLines.front();
	mLines.pop();

	return line;
}

void DialogueSystem::Initialize()
{
	auto& uiManager = based::Engine::Instance().GetUiManager();
	auto context = uiManager.GetContext("main");

	uiManager.SetPathPrefix("Assets/UI/");
	mDocument = uiManager.LoadWindow("DialogueBox", context);
	mDocument->document->Hide();

	mDialogueSounds = FMODSystem::CreateFMODEvent("event:/Dialogue/Dialogue");

	mSpeakers.emplace_back();
	mSpeakers.emplace_back(SpeakerSettings{ .MinPitch = 0.8f, .MaxPitch = 1.f });
}

void DialogueSystem::Update(float deltaTime)
{
	using namespace based;

	if (!mDocument || !mInDialogue) return;

	// Typewriter animation, reveal another letter once enough
	// time has passed since the last letter
	if (mDocument->document && mDocument->document->IsVisible() && mIsTyping)
	{
		if (core::Time::GetUnscaledTime() - mLastUpdate >= mTextSpeed)
		{
			if (mProgress < mCurrentLine.size())
			{
				auto elem = mDocument->document->GetElementById("dialogue-body");
				mLastUpdate = core::Time::GetUnscaledTime();

				auto current = elem->GetInnerRML();
				char currentChar = mCurrentLine[mProgress];
				elem->SetInnerRML(current + currentChar);

				SpeakerSettings speaker = mSpeakers[mCurrentSpeaker];

				if (mProgress % speaker.Frequency == 0)
				{
					PlayCharacterSound(speaker, currentChar);
				}

				++mProgress;
			}
			else mIsTyping = false;
		}
	}
	else mIsTyping = false;

	// Skip text to end, or show next line if already at end
	if (input::Mouse::ButtonDown(BASED_INPUT_MOUSE_LEFT))
	{
		if (mIsTyping)
		{
			mIsTyping = false;
			auto elem = mDocument->document->GetElementById("dialogue-body");
			elem->SetInnerRML(mCurrentLine);
		}
		else ShowNextLine();
	}
}

void DialogueSystem::SetCurrentDialogue(const std::string& path)
{
	mInDialogue = true;
	mCurrentDialogue = DialogueSet(path); // Loads a given dialogue file
	ShowNextLine();

	GameSystems::SetPlayerMouseLookEnabled(false);
	GameSystems::SetPlayerMovementEnabled(false);
	based::input::Mouse::SetCursorVisible(true);
}

void DialogueSystem::CloseCurrentDialogue()
{
	mInDialogue = false;
	if (mDocument)
		mDocument->document->Hide();

	GameSystems::SetPlayerMouseLookEnabled(true);
	GameSystems::SetPlayerMovementEnabled(true);
	based::input::Mouse::SetCursorVisible(false);
}

void DialogueSystem::ShowNextLine()
{
	if (!mInDialogue) return;

	auto line = mCurrentDialogue.PopNextLine();

	if (line.empty())
	{
		CloseCurrentDialogue();
		if (mOnDialogueFinished.data())
			mOnDialogueFinished();
		return;
	}

	if (std::isdigit(line[0]))
	{
		mCurrentSpeaker = based::math::Clamp((int)(line[0] - '0'), 0, (int)mSpeakers.size() - 1);
		line = line.substr(1);
	}
	else mCurrentSpeaker = 0;

	// Clear text to prepare for next line
	auto elem = mDocument->document->GetElementById("dialogue-body");
	elem->SetInnerRML("");

	// If the line contains a : then the text preceding that is a speaker tag,
	// so that gets shown, otherwise the speaker box is hidden
	auto speakerBox = mDocument->document->GetElementById("speaker-box");
	if (auto it = line.find(":"); it != std::string::npos)
	{
		auto speaker = line.substr(0, it);
		line = line.substr(it + 1);
		auto spkElem = mDocument->document->GetElementById("speaker");
		spkElem->SetInnerRML(speaker);
		speakerBox->SetClass("hide", false);
	} else
	{
		speakerBox->SetClass("hide", true);
	}

	if (!mDocument->document->IsVisible())
		mDocument->document->Show();

	// Reset typewriter variables
	mCurrentLine = line;
	mProgress = 0;
	mLastUpdate = based::core::Time::GetUnscaledTime();
	mIsTyping = true;
}

void DialogueSystem::PlayCharacterSound(SpeakerSettings& speaker, char character)
{
	/*auto consonantEvent = FMODSystem::CreateFMODEvent(speaker.ConsonantEventPath);
	auto vowelEvent = FMODSystem::CreateFMODEvent(speaker.VowelEventPath);
	consonantEvent->setPitch(1.2f);
	if (currentChar == 'a' || currentChar == 'e' || currentChar == 'i' || currentChar == 'o'
		|| currentChar == 'u')
	{
		FMODSystem::SetEventParameter(dialogueEvent, "Dialogue", 1);
	} else if (currentChar == '.' || currentChar == '?' || currentChar == '!')
	{
		FMODSystem::SetEventParameter(dialogueEvent, "Dialogue", 2);
	}
	else
	{
		FMODSystem::SetEventParameter(dialogueEvent, "Dialogue", 0);
	}

	dialogueEvent->start();*/
}
