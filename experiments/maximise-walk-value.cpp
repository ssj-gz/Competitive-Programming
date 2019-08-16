// Simon St James (ssjgz) - 2019-08-16
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <limits>

#include <cassert>

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

const auto maxCost = 1000;

struct CostInfo
{
    bool canBeGenerated = false;
    int nextLowestGeneratableCost = -1;
};

struct Node
{
    vector<Node*> neighbours;
    int cost = 0;

    vector<vector<CostInfo>> generatableCostsOnPathToSpecialNode;
};

void doDfsFromSpecialNode(Node* currentNode, Node* parent, const int sourceSpecialNodeId)
{
    auto& generatableCostsOnPathToSpecialNode = currentNode->generatableCostsOnPathToSpecialNode[sourceSpecialNodeId];
    if (parent)
    {
        // We can generate precisely the set of costs that our parent did, just by not including our cost in the subset.
        generatableCostsOnPathToSpecialNode = parent->generatableCostsOnPathToSpecialNode[sourceSpecialNodeId];
        assert(generatableCostsOnPathToSpecialNode.size() == maxCost + 1);
        // ... but we can also generate "cost from source special node to parent" + "currentNode->cost", by
        // including currentNode in the subset!
        for (int i = maxCost; i >= currentNode->cost; i--)
        {
            if (generatableCostsOnPathToSpecialNode[i - currentNode->cost].canBeGenerated)
            {
                generatableCostsOnPathToSpecialNode[i].canBeGenerated = true;
            }
        }
    }
    else
    {
        // The only ones we can generate are 0 (by not including currentNode) and currentNode->cost
        // (by including currentNode!)
        generatableCostsOnPathToSpecialNode.resize(maxCost + 1);
        generatableCostsOnPathToSpecialNode[0].canBeGenerated = true;
        generatableCostsOnPathToSpecialNode[currentNode->cost].canBeGenerated = true;
    }

    // Update nextLowestGeneratableCost for our CostInfo.
    int nextLowestGeneratableCost = -1;
    for (int cost = 0; cost <= maxCost; cost++)
    {
        generatableCostsOnPathToSpecialNode[cost].nextLowestGeneratableCost = nextLowestGeneratableCost;
        if (generatableCostsOnPathToSpecialNode[cost].canBeGenerated)
            nextLowestGeneratableCost = cost;
    }

    // Recurse.
    for (auto child : currentNode->neighbours)
    {
        if (child == parent)
            continue;

        doDfsFromSpecialNode(child, currentNode, sourceSpecialNodeId);
    }
}

struct PVValue
{
    int minDiff = numeric_limits<int>::max();
    int maxSumForMinDiff = -1;
};

void updatePVValue(PVValue& currentPVValue, const PVValue& newPVValue)
{
    if (currentPVValue.minDiff < newPVValue.minDiff)
        return;
    if (currentPVValue.minDiff > newPVValue.minDiff)
    {
        currentPVValue = newPVValue;
    }
    else
    {
        assert(currentPVValue.minDiff == newPVValue.minDiff);
        currentPVValue.maxSumForMinDiff = max(currentPVValue.maxSumForMinDiff, newPVValue.maxSumForMinDiff);
    }
}

PVValue findPVValue(const vector<CostInfo>& costInfo1, const vector<CostInfo>& costInfo2, const int costLimit)
{
    PVValue result;

    auto findLargestGeneratableCost = [costLimit](const vector<CostInfo>& costInfo)
    {
        int largestCost = costLimit;
        if (!costInfo[largestCost].canBeGenerated)
            largestCost = costInfo[largestCost].nextLowestGeneratableCost;

        // We can always generate "0", so largestCost shoud never be -1.
        assert(largestCost != -1);

        return largestCost;
    };

    auto incorporateNewCostPair = [&result](const int costIn1, const int costIn2)
    {
        PVValue newPVValue = { abs(costIn1 - costIn2), costIn1 + costIn2 };
        updatePVValue(result, newPVValue);
    };

    int costIn1 = findLargestGeneratableCost(costInfo1);

    int nextCostIn2AtLeastCostIn1 = findLargestGeneratableCost(costInfo2);
    int nextCostIn2LessThanCostIn1 = findLargestGeneratableCost(costInfo2);

    while (costIn1 != -1)
    {
        while (nextCostIn2LessThanCostIn1 != -1 && nextCostIn2LessThanCostIn1 >= costIn1)
            nextCostIn2LessThanCostIn1 = costInfo2[nextCostIn2LessThanCostIn1].nextLowestGeneratableCost;
        while (nextCostIn2AtLeastCostIn1 != -1 && costInfo2[nextCostIn2AtLeastCostIn1].nextLowestGeneratableCost != -1 && costInfo2[nextCostIn2AtLeastCostIn1].nextLowestGeneratableCost >= costIn1)
            nextCostIn2AtLeastCostIn1 = costInfo2[nextCostIn2AtLeastCostIn1].nextLowestGeneratableCost;

        incorporateNewCostPair(costIn1, nextCostIn2LessThanCostIn1);
        incorporateNewCostPair(costIn1, nextCostIn2AtLeastCostIn1);

        costIn1 = costInfo1[costIn1].nextLowestGeneratableCost;
    }
    

    return result;
}

int findBestPVValueForQuery(const Node* sourceNode, const Node* destNode, const int costLimit, const vector<Node*>& specialNodes)
{
    PVValue result;

    for (int specialNodeIndex = 0; specialNodeIndex < specialNodes.size(); specialNodeIndex++)
    {
        const auto resultWithThisPivot = findPVValue(sourceNode->generatableCostsOnPathToSpecialNode[specialNodeIndex], 
                                                     destNode->generatableCostsOnPathToSpecialNode[specialNodeIndex],
                                                     costLimit);

        updatePVValue(result, resultWithThisPivot);
    }
    return result.maxSumForMinDiff;
}

void buildLookupTables(vector<Node>& nodes, const vector<Node*>& specialNodes)
{
    for (auto& node : nodes)
    {
        node.generatableCostsOnPathToSpecialNode.resize(specialNodes.size());
    }

    for (int specialNodeIndex = 0; specialNodeIndex < specialNodes.size(); specialNodeIndex++)
    {
        doDfsFromSpecialNode(specialNodes[specialNodeIndex], nullptr, specialNodeIndex);
    }
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);

    const int numNodes = read<int>();
    const int numSpecialNodes = read<int>();
    const int numQueries = read<int>();

    vector<Node> nodes(numNodes);

    for (int i = 0; i < numNodes - 1; i++)
    {
        const int v = read<int>() - 1;

        nodes[v].neighbours.push_back(&(nodes[i + 1]));
        nodes[i + 1].neighbours.push_back(&(nodes[v]));
    }

    for (int i = 0; i < numNodes; i++)
    {
        nodes[i].cost = read<int>();
    }

    vector<Node*> specialNodes;

    for (int i = 0; i < numSpecialNodes; i++)
    {
        specialNodes.push_back(&(nodes[read<int>() - 1]));
    }

    buildLookupTables(nodes, specialNodes);

    for (int q = 0; q < numQueries; q++)
    {
        Node* sourceNode = &(nodes[read<int>() - 1]);
        Node* destNode = &(nodes[read<int>() - 1]);
        int maxCost = read<int>();

        const auto queryResultOptimised = findBestPVValueForQuery(sourceNode, destNode, maxCost, specialNodes);
        cout << queryResultOptimised << endl;
    }
    assert(cin);
}


