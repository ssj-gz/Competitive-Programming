// O(N^2) solution; expected to pass Subtasks #1 and #2
#include <iostream>
#include <vector>
#include <map>
#include <cassert>

using namespace std;

struct Node
{
    int numCounters = -1;
    vector<Node*> neighbours;

    int grundyNumberIfRoot = 0;
};

int grundyContributionFromSubtree(Node* currentNode, Node* parentNode, int depth)
{
    int grundyContribution = 0;
    if (currentNode->numCounters % 2 == 1)
        grundyContribution ^= depth;

    for (const auto child : currentNode->neighbours)
    {
        if (child == parentNode)
            continue;

        grundyContribution ^= grundyContributionFromSubtree(child, currentNode, depth + 1);
    }

    return grundyContribution;
}

int main()
{
    auto readInt = [](){ int x; cin >> x; return x; };

    const auto numTestcases = readInt();
    for (int t = 0; t < numTestcases; t++)
    {
        const auto numNodes = readInt();
        vector<Node> nodes(numNodes);

        for (auto edgeNum = 0; edgeNum < numNodes - 1; edgeNum++)
        {
            const auto node1Index = readInt() - 1;
            const auto node2Index = readInt() - 1;

            nodes[node1Index].neighbours.push_back(&(nodes[node2Index]));
            nodes[node2Index].neighbours.push_back(&(nodes[node1Index]));
        }

        for (auto& node : nodes)
        {
            node.numCounters = readInt();
        }

        int64_t MOD = 1'000'000'007;
        int64_t result = 0;
        int64_t powerOf2 = 2;
        for (auto& rootNode : nodes)
        {
            const bool bobWins = (grundyContributionFromSubtree(&rootNode, nullptr, 0) == 0);
            if (bobWins)
            {
                result = (result + powerOf2) % MOD;
            }
            powerOf2 = (powerOf2 * 2) % MOD;
        }
        cout << result << endl;
    }

}
