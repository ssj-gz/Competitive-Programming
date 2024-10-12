#include <iostream>
#include <regex>
#include <set>
#include <vector>

#include <cassert>

using namespace std;

struct Step
{
    char label = '\0';

    set<Step*> blockedBy;
    set<Step*> blocking;
};

int main()
{
    std::regex requirementRegex(R"(^Step ([A-Z]) must be finished before step ([A-Z]) can begin\.$)");
    string requirementLine;
    std::map<char, Step> stepForLabel;
    while (getline(cin, requirementLine))
    {
        std::smatch requirementMatch;
        const bool matchSuccessful = std::regex_match(requirementLine, requirementMatch, requirementRegex);
        assert(matchSuccessful);
        const auto mustBeDoneFirstLabel = std::string(requirementMatch[1]).front();
        const auto doneAfterlabel = std::string(requirementMatch[2]).front();
        std::cout << "mustBeDoneFirstLabel: " << mustBeDoneFirstLabel << " doneAfterlabel: " << doneAfterlabel << std::endl;
        stepForLabel[mustBeDoneFirstLabel].blocking.insert(&(stepForLabel[doneAfterlabel]));
        stepForLabel[doneAfterlabel].blockedBy.insert(&(stepForLabel[mustBeDoneFirstLabel]));
        stepForLabel[mustBeDoneFirstLabel].label = mustBeDoneFirstLabel;
        stepForLabel[doneAfterlabel].label = doneAfterlabel;
    }

    struct StepComparator
    {
        bool operator()(const Step* lhs, const Step* rhs) const
        {
            if (lhs->blockedBy.size() != rhs->blockedBy.size())
                return lhs->blockedBy.size() < rhs->blockedBy.size();
            return lhs->label < rhs->label;
        };
    };

    set<Step*, StepComparator> sortedSteps{StepComparator()};
    for (auto& [label, step] : stepForLabel)
        sortedSteps.insert(&step);
    std::cout << "sortedSteps: " << std::endl;
    for (const auto* step : sortedSteps)
    {
        std::cout << " " << step->label << " blockedBy: ";
        for (auto* blockingStep : step->blockedBy)
        {
            std::cout << blockingStep->label << " ";
        }
        std::cout << std::endl;
    }

    string stepLabelOrder;
    while (!sortedSteps.empty())
    {
        std::cout << "#sortedSteps: " << sortedSteps.size() << std::endl;
        auto* nextStep = *sortedSteps.begin();
        std::cout << " nextStep: " << nextStep->label << std::endl;
        stepLabelOrder.push_back(nextStep->label);
        assert(nextStep->blockedBy.empty());
        sortedSteps.erase(nextStep);
        for (auto* dependee : nextStep->blocking)
        {
            sortedSteps.erase(dependee);
            assert(dependee->blockedBy.contains(nextStep));
            dependee->blockedBy.erase(nextStep);
            sortedSteps.insert(dependee);
        }
    }

    std::cout << "order: " << stepLabelOrder << std::endl;

    return 0;
}
