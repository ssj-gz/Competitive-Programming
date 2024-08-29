#include <iostream>
#include <map>
#include <deque>
#include <vector>
#include <regex>

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
    int numSteps = 0;
    bool done = false;
    while (!done)
    {
        if ((numSteps % 10'000) == 0)
            std::cout << "numSteps: " << numSteps << std::endl;
        for (const auto direction : directions)
        {
            bool allReachedEnd = true;
            for (auto* currentNode : currentNodes)
            {
                assert(currentNode);
                if (currentNode->label.back() != 'Z')
                {
                    allReachedEnd = false;
                    break;
                }

            }
            if (allReachedEnd)
            {
                std::cout << "Done" << std::endl;
                done = true;
                break;

            }
            for (auto& currentNode : currentNodes)
            {
                if (direction == 'L')
                    currentNode = nodeByLabel[currentNode->leftLabel];
                else if (direction == 'R')
                    currentNode = nodeByLabel[currentNode->rightLabel];
                else
                    assert(false);
            }
            numSteps++;
        }
    }
    std::cout << "numSteps: " << numSteps << std::endl;
}
