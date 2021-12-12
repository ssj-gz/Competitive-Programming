#include <iostream>
#include <deque>
#include <regex>
#include <map>
#include <clocale>
#include <cassert>


using namespace std;

struct Node
{
    bool isEndNode = false;
    int maxVisitCount = -1;
    vector<Node*> neighbours;
    string nodeName;

    int visitCount = 0;
};

void countPaths(Node* currentNode, vector<Node*>& nodeStack, int64_t& numPathsFound, bool canRevisitSmallCave)
{
    currentNode->visitCount++;
    nodeStack.push_back(currentNode);
    const bool isSmallCave = (currentNode->maxVisitCount == 2);
    bool skipNode = false;
    if (isSmallCave && currentNode->visitCount == currentNode->maxVisitCount)
    {
        if (!canRevisitSmallCave)
            skipNode = true;
        canRevisitSmallCave = false;
    }
    if (!skipNode)
    {
        if (currentNode->isEndNode)
        {
            numPathsFound++;
            cout << "path: ";
            for (const auto node : nodeStack)
            {
                cout << node->nodeName;
                if (node != currentNode)
                    cout << "->";
            }
            cout << endl;
        }
        else if (currentNode->maxVisitCount != -1 && currentNode->visitCount > currentNode->maxVisitCount)
        {
            // Do nothing.
        }
        else
        {
            for (auto neighbour : currentNode->neighbours)
            {
                countPaths(neighbour, nodeStack, numPathsFound, canRevisitSmallCave);
            }
        }
    }
    nodeStack.pop_back();
    currentNode->visitCount--;
    return;
}


int main()
{
    deque<Node> nodes;
    string pathDescLine;
    
    map<string, Node*> nodesByName;

    auto getNode = [&nodesByName,&nodes](const string& nodeName)
            {
                if (nodesByName.find(nodeName) == nodesByName.end())
                {
                    nodes.push_back({false, false, {}, nodeName});
                    auto newNode = &(nodes.back());
                    nodesByName[nodeName] = newNode;
                    newNode->maxVisitCount = any_of(nodeName.begin(), nodeName.end(), static_cast<int(*)(int)>(isupper)) ? -1 : 2;
                    return newNode;
                }
                return nodesByName[nodeName];
            };

    while (getline(cin, pathDescLine))
    {
        static regex pathDescRegex(R"((\w+)-(\w+)\s*)");
        std::smatch pathDescMatch;
        cout << "Line: " << pathDescLine << endl;
        if (regex_match(pathDescLine, pathDescMatch, pathDescRegex))
        {
            const string& nodeName1 = pathDescMatch[1];
            const string& nodeName2 = pathDescMatch[2];
            auto node1 = getNode(nodeName1);
            auto node2 = getNode(nodeName2);
            node1->neighbours.push_back(node2);
            node2->neighbours.push_back(node1);
        }
        else
        {
            assert(false);
        }
    }
    auto startNode = getNode("start");
    auto endNode = getNode("end");
    startNode->maxVisitCount = 1;
    endNode->isEndNode = true;
    endNode->maxVisitCount = 1;

    for (const auto& node : nodes)
    {
        cout << "node: " << node.nodeName << " maxVisitCount: " << node.maxVisitCount << endl;
        for (const auto& neighbour : node.neighbours)
        {
            cout << " " << neighbour->nodeName << endl;
        }
    }

    vector<Node*> nodeStack;
    int64_t numPathsFound = 0;
    countPaths(startNode, nodeStack, numPathsFound, true);
    cout << numPathsFound << endl;
    
}
