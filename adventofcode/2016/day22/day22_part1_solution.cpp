#include <iostream>
#include <regex>
#include <set>

#include <cassert>

using namespace std;

int main()
{
    // root@ebhq-gridcenter# df -h
    string dfCommand;
    getline(cin, dfCommand);
    // Filesystem              Size  Used  Avail  Use%
    string headers;
    getline(cin, headers);

    struct Node
    {
        int64_t x = -1;
        int64_t y = -1;
        int64_t size = -1;
        int64_t used = -1;
        int64_t avail = -1;
        int64_t usedPercentage = -1;

        vector<Node*> neighbours;
    };

    std::regex dfRegex(R"(^/dev/grid/node-x(\d+)-y(\d+)\s*(\d+)T\s*(\d+)T\s*(\d+)T\s*(\d+)%$)");
    string dfLine;
    vector<Node> nodes;
    while (getline(cin, dfLine))
    {
        std::smatch dfMatch;
        const bool matchSuccessful = std::regex_match(dfLine, dfMatch, dfRegex);
        assert(matchSuccessful);
        Node node;
        node.x = std::stoll(dfMatch[1]);
        node.y = std::stoll(dfMatch[2]);
        node.size = std::stoll(dfMatch[3]);
        node.used = std::stoll(dfMatch[4]);
        node.avail = std::stoll(dfMatch[5]);
        node.usedPercentage = std::stoll(dfMatch[6]);
        nodes.push_back(node);
    }

    for (auto& node : nodes)
    {
        for (auto& otherNode : nodes)
        {
            if (abs(otherNode.x - node.x) + abs(otherNode.y - node.y) == 1)
            {
                node.neighbours.push_back(&otherNode);
                std::cout << " connected node: " << node.x << "," << node.y << " with node: " << otherNode.x << ", " << otherNode.y << std::endl;
            }
        }
    }
    std::cout << "# nodes: " << nodes.size() << std::endl;

#if 0
    set<Node*> inAnyComponent;

    int64_t result = 0;
    for (auto& startNode : nodes)
    {
        if (inAnyComponent.contains(&startNode))
            continue;
        set<Node*> connectedComponent = { &startNode };
        vector<Node*> toExplore = { &startNode };
        while (!toExplore.empty())
        {
            vector<Node*> nextToExplore;
            for (auto* node : toExplore)
            {
                inAnyComponent.insert(node);
                for (auto* neighbour : node->neighbours)
                {
                    if (!connectedComponent.contains(neighbour))
                    {
                        connectedComponent.insert(neighbour);
                        nextToExplore.push_back(neighbour);
                    }
                }
            }

            toExplore = nextToExplore;
        }
        std::cout << "connectedComponent size: " << connectedComponent.size() << std::endl;

        for (auto* nodeA : connectedComponent)
        {
            for (auto* nodeB : connectedComponent)
            {
                if ((nodeA != nodeB) && (nodeA->used > 0) && (nodeA->used <= nodeB->avail))
                {
                    result++;
                }
            }
        }
    }
#endif
    int64_t result = 0;
    for (auto& nodeA : nodes)
    {
        for (auto& nodeB : nodes)
        {
            if ((&nodeA != &nodeB) && (nodeA.used > 0) && (nodeA.used <= nodeB.avail))
            {
                result++;
            }
        }
    }
    std::cout << "result: " << result << std::endl;
    return 0;
}
