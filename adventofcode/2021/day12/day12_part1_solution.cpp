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
    bool canBeRevisited = false;
    vector<Node*> neighbours;
    string nodeName;

    bool visited = false;
};

void countPaths(Node* currentNode, vector<Node*>& nodeStack, int64_t& numPathsFound)
{
    if (currentNode->isEndNode)
    {
        numPathsFound++;
        cout << "path: ";
        for (const auto node : nodeStack)
        {
            cout << node->nodeName << "->";
        }
        cout << currentNode->nodeName;
        cout << endl;
        return;
    }
    currentNode->visited = true;
    nodeStack.push_back(currentNode);
    for (auto neighbour : currentNode->neighbours)
    {
        if (!neighbour->visited || neighbour->canBeRevisited)
        {
            countPaths(neighbour, nodeStack, numPathsFound);
        }
    }
    nodeStack.pop_back();
    currentNode->visited = false;
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
                    newNode->canBeRevisited = any_of(nodeName.begin(), nodeName.end(), static_cast<int(*)(int)>(isupper));
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
    endNode->isEndNode = true;

    for (const auto& node : nodes)
    {
        cout << "node: " << node.nodeName << " canBeRevisited: " << node.canBeRevisited << endl;
        for (const auto& neighbour : node.neighbours)
        {
            cout << " " << neighbour->nodeName << endl;
        }
    }

    vector<Node*> nodeStack;
    int64_t numPathsFound = 0;
    countPaths(startNode, nodeStack, numPathsFound);
    cout << numPathsFound << endl;
    
}
