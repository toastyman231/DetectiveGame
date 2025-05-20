#pragma once
#include "based/input/basedinput.h"

namespace based::managers
{
	struct DocumentInfo;
}

enum class WordType
{
	Person,
	Place,
	Thing,
	Verb,
	Number
};

struct SolutionWord
{
	std::string Word;
	WordType Type;

	bool operator== (const SolutionWord& other) const
	{
		return Word == other.Word && Type == other.Type;
	}
};

struct AnyOfWords
{
	std::vector<std::string> PossibleWords;

	bool operator== (const std::string& other)
	{
		return std::find(PossibleWords.begin(), PossibleWords.end(), other) != PossibleWords.end();
	}
	bool operator!= (const std::string& other)
	{
		return !(*this == other);
	}
};

struct ISolutionPanel
{
	ISolutionPanel(const std::string& id) : PanelID(id) {}
	virtual ~ISolutionPanel() = default;

	virtual bool CheckSolution() = 0;
	virtual bool CanDropElement(Rml::Element* target, Rml::Element* drag_element) = 0;
	virtual bool IsAnySlotUnfilled() = 0;
	virtual void SetupPanel(Rml::ElementDocument* document) = 0;
	virtual void OnSolutionElementDropped(std::string slotID, void* element) = 0;
	virtual void OnPanelSolved();
	virtual void OnPanelIncorrect();

	bool IsLocked = false;
	std::string PanelID;
};

struct WordSolution
{
	std::string CurrentWord;
	AnyOfWords SolutionWords;
};

struct WordSolutionPanel : public ISolutionPanel
{
	explicit WordSolutionPanel(const std::string& id, const std::string& solutionPath);

	bool CheckSolution() override;
	void OnSolutionElementDropped(std::string slotID, void* element) override;
	void SetupPanel(Rml::ElementDocument* document) override;
	bool CanDropElement(Rml::Element* target, Rml::Element* drag_element) override;
	bool IsAnySlotUnfilled() override;

	std::unordered_map<std::string, WordSolution> WorkingSolution;
};

class SolutionPanelSystem : public Rml::EventListener
{
public:
	void Initialize();

	void Update(float deltaTime);

	void AddWord(SolutionWord word);

	void AddSolutionPanel(std::string panelID, ISolutionPanel* panel);

	void ProcessEvent(Rml::Event& event) override;

	void RegisterDraggableContainer(Rml::Element* element);
	void SetClassByType(Rml::ElementPtr& wordElement, SolutionWord word);

	void SetLocked(bool locked) { mIsLocked = locked; }

	static std::string GetWordTypeFromClass(const std::string& classNames);
private:
	void AddWordInternal(SolutionWord word);
	bool CheckSolution();
	void OnInput(const based::input::InputAction& action);

	bool mIsPanelGroupShown = false;
	bool mIsLocked = false;
	based::managers::DocumentInfo* mPanelGroup = nullptr;

	std::vector<SolutionWord> mWords;

	std::unordered_map<std::string, ISolutionPanel*> mSolutionPanels;
};
