#pragma once

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
	std::vector<SolutionWord> PossibleWords;

	bool operator== (const SolutionWord& other)
	{
		return std::find(PossibleWords.begin(), PossibleWords.end(), other) != PossibleWords.end();
	}
};

struct ISolutionPanel
{
	ISolutionPanel(const std::string& id) : PanelID(id) {}
	virtual ~ISolutionPanel() = default;

	virtual bool CheckSolution() = 0;
	virtual bool CanDropElement(Rml::Element* target, Rml::Element* drag_element) = 0;
	virtual void SetupPanel(Rml::ElementDocument* document) = 0;
	virtual void OnSolutionElementDropped(std::string slotID, void* element) = 0;
	virtual void OnPanelSolved();

	bool IsLocked = false;
	std::string PanelID;
};

struct WordSolutionPanel : public ISolutionPanel
{
	explicit WordSolutionPanel(const std::string& id)
		: ISolutionPanel(id)
	{}

	bool CheckSolution() override;
	void OnSolutionElementDropped(std::string slotID, void* element) override;
	void SetupPanel(Rml::ElementDocument* document) override;
	bool CanDropElement(Rml::Element* target, Rml::Element* drag_element) override;

	std::unordered_map<std::string, std::string> WorkingSolution;
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

	static std::string GetWordTypeFromClass(const std::string& classNames);
private:
	void AddWordInternal(SolutionWord word);
	bool CheckSolution();

	bool mIsPanelGroupShown = false;
	based::managers::DocumentInfo* mPanelGroup = nullptr;

	std::vector<SolutionWord> mWords;

	std::unordered_map<std::string, ISolutionPanel*> mSolutionPanels;
};
