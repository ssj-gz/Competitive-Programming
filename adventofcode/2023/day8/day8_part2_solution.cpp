#include <iostream>
#include <map>
#include <deque>
#include <vector>
#include <regex>
#include <numeric>

#include <cassert>

using namespace std;

int main()
{
    struct Node
    {
        string label;
        string leftLabel;
        string rightLabel;
    };
    string directions;
    getline(cin, directions);
    assert(!directions.empty());

    string nodeLine;
    std::regex nodeMapRegex(R"(^\s*([A-Z]+)\s*=\s*\(\s*([A-Z]+)\s*,\s*([A-Z]+)\s*\))");
    deque<Node> nodes;
    map<string, Node*> nodeByLabel;
    while (getline(cin, nodeLine))
    {
        if (nodeLine.empty())
            continue;
        std::cout << "nodeLine: " << nodeLine << std::endl;
        std::smatch nodeMapMatch;
        const bool matchSuccessful = std::regex_match(nodeLine, nodeMapMatch, nodeMapRegex);
        assert(matchSuccessful);
        std::cout << nodeMapMatch[1] << " -> " << nodeMapMatch[2] << " or " << nodeMapMatch[3] << std::endl;
        nodes.push_back({nodeMapMatch[1], nodeMapMatch[2], nodeMapMatch[3]});
        nodeByLabel[nodes.back().label] = &nodes.back();
    }

    vector<Node*> currentNodes;
    vector<Node*> targetNodes;
    for (auto& node : nodes)
    {
        if (node.label.back() == 'A')
            currentNodes.push_back(&node);
        if (node.label.back() == 'Z')
            targetNodes.push_back(&node);
    }
    std::cout << "#currentNodes: " << currentNodes.size() << std::endl;

    int64_t result = 1;
    for (auto* currentNode : currentNodes)
    {
        std::cout << "starting node: " << currentNode->label << std::endl;
        int numSteps = 0;
        bool done = false;
        struct State
        {
            Node *node = nullptr;
            int directionIndex = -1;
            auto operator<=>(const State& other) const = default;
        };
        map<State, int> earliestStepToReach;
        struct StartNodeInfo
        {
            int cycleLength = -1;
            int preCycleLength = -1;
            int posOfFinal = -1;
        } startNodeInfo;
        while (!done)
        {
            for (int directionIndex = 0; directionIndex < directions.size(); directionIndex++)
            {
                if (currentNode->label.back() == 'Z')
                {
                    std::cout << " reached final node: " << currentNode->label << " at step: " << numSteps << std::endl;
                    startNodeInfo.posOfFinal = numSteps;
                }
                auto direction = directions[directionIndex];
                State currentState = {currentNode, directionIndex};
                if (!earliestStepToReach.contains(currentState))
                {
                    earliestStepToReach[currentState] = numSteps;
                }
                else
                {
                    std::cout << "Reached repetition of state at numSteps: " << numSteps << " first reached state at step: " << earliestStepToReach[currentState] << std::endl;
                    startNodeInfo.preCycleLength = earliestStepToReach[currentState];
                    startNodeInfo.cycleLength = numSteps - earliestStepToReach[currentState];
                    assert(startNodeInfo.posOfFinal == startNodeInfo.cycleLength);
                    result = std::lcm(result, startNodeInfo.cycleLength);
                    done = true;
                    break;
                }
                if (direction == 'L')
                    currentNode = nodeByLabel[currentNode->leftLabel];
                else if (direction == 'R')
                    currentNode = nodeByLabel[currentNode->rightLabel];
                else
                    assert(false);
                numSteps++;
            }
        }
    }
    std::cout << "result: " << result << std::endl;
}
