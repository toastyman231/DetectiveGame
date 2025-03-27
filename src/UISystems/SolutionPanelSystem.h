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
};

class SolutionPanelSystem
{
public:
	void Initialize();

	void Update(float deltaTime);

	void AddWord(SolutionWord word);

private:
	void AddWordInternal(SolutionWord word);

	bool mIsPanelGroupShown = false;
	based::managers::DocumentInfo* mPanelGroup = nullptr;

	std::vector<SolutionWord> mWords;
};
