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

struct Worker
{
    int id = -1;
    Step* stepWorkingOn = nullptr;
    int countdownToCompletion = -1;
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

    constexpr int numWorkers = 5;
    vector<Worker> workers(numWorkers);
    for (int id = 1; id <= numWorkers; id++)
            workers[id - 1].id = id;

    int second = 0;
    while (!sortedSteps.empty() || std::count_if(workers.begin(), workers.end(), [](const auto& worker) { return worker.stepWorkingOn != nullptr; }) > 0)
    {
        // Allocate any eligible unallocated steps to any free workers.
        while (!sortedSteps.empty())
        {
            Worker* firstFreeWorker = nullptr;
            if (auto firstFreeWorkerIter = std::find_if(workers.begin(), workers.end(), [](const auto& worker) { return worker.stepWorkingOn == nullptr; }); firstFreeWorkerIter != workers.end())
                firstFreeWorker = &(*firstFreeWorkerIter);
            if (firstFreeWorker == nullptr)
            {
                // No workers; abort allocation.
                break;
            }
            else
            {
                auto* nextStep = *sortedSteps.begin();
                if (nextStep->blockedBy.empty())
                {
                    sortedSteps.erase(nextStep);
                    std::cout << " nextStep: " << nextStep->label << " assigned to worker: " << firstFreeWorker->id << std::endl;
                    firstFreeWorker->stepWorkingOn = nextStep;
                    firstFreeWorker->countdownToCompletion = nextStep->label - 'A' + 61;
                }
                else
                {
                    // No eligible steps; abort allocation.
                    break;
                }
            }
        }
        std::cout << "Before end of second: " << second << std::endl;
        int workerNum = 1;
        for (auto& worker : workers)
        {
            std::cout << "Worker #" << workerNum << " working on: " << (worker.stepWorkingOn != nullptr ? worker.stepWorkingOn->label : '.') << std::endl;
            workerNum++;
        }

        for (auto& worker : workers)
        {
            if (worker.stepWorkingOn != nullptr)
            {
                worker.countdownToCompletion--; 
                if (worker.countdownToCompletion == 0)
                {
                    auto* stepJustFinished = worker.stepWorkingOn;
                    std::cout << " step " << stepJustFinished->label << " completed" << std::endl;
                    for (auto* dependee : stepJustFinished->blocking)
                    {
                        sortedSteps.erase(dependee);
                        assert(dependee->blockedBy.contains(stepJustFinished));
                        dependee->blockedBy.erase(stepJustFinished);
                        sortedSteps.insert(dependee);
                    }
                    worker.stepWorkingOn = nullptr;
                    worker.countdownToCompletion = -1;
                }
            }
        }
        second++;
    }
    std::cout << "#seconds: " << second << std::endl;

    return 0;
}
