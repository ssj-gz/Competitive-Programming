#include <iostream>
#include <map>
#include <deque>
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

    Node* currentNode = nodeByLabel["AAA"];
    Node* targetNode = nodeByLabel["ZZZ"];
    assert(targetNode);
    int numSteps = 0;
    while (currentNode != targetNode)
    {
        for (const auto direction : directions)
        {
            assert(currentNode);
            if (currentNode == targetNode)
            {
                std::cout << "Done" << std::endl;
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
    std::cout << "numSteps: " << numSteps << std::endl;
}
