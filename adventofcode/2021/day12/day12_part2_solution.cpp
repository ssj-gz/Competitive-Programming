#include <iostream>
#include <deque>
#include <regex>
#include <map>
#include <clocale>
#include <cassert>


using namespace std;

struct Node
{
    string nodeName;
    bool isEndNode = false;
    vector<Node*> neighbours;

    int maxVisitCount = -1;
    int visitCount = 0;
};

void countPaths(Node* currentNode, int64_t& numPathsFound, bool canRevisitSmallCave)
{
    currentNode->visitCount++;

    bool exploreFromThisNode = true;

    const bool isSmallCave = (currentNode->maxVisitCount == 2);
    if (isSmallCave && currentNode->visitCount == currentNode->maxVisitCount)
    {
        if (!canRevisitSmallCave)
            exploreFromThisNode = false;
        canRevisitSmallCave = false;
    }
    if (currentNode->maxVisitCount != -1 && currentNode->visitCount > currentNode->maxVisitCount)
        exploreFromThisNode = false;
    if (currentNode->isEndNode)
    {
        numPathsFound++;
        exploreFromThisNode = false;
    }

    if (exploreFromThisNode)
    {
        for (auto neighbour : currentNode->neighbours)
        {
            countPaths(neighbour, numPathsFound, canRevisitSmallCave);
        }
    }

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
                    nodes.push_back({nodeName});
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
    startNode->maxVisitCount = 1;

    auto endNode = getNode("end");
    endNode->isEndNode = true;
    endNode->maxVisitCount = 1;

    int64_t numPathsFound = 0;
    countPaths(startNode, numPathsFound, true);
    cout << numPathsFound << endl;
    
}
