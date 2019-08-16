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

const auto maxCostSum = 1000;

struct CostInfo
{
    bool canBeGenerated = false;
    int nextLowestGeneratableCost = -1;
};

struct Node
{
    vector<Node*> neighbours;
    int cost = 0;

    // costSumsOnPathToSpecialNode[specialNodeIndex][cost].canBeGenerated is true
    // if and only if the path from specialNodes[specialNodeIndex] to this node
    // has a cost subset summing to "cost".
    vector<vector<CostInfo>> costSumsOnPathToSpecialNode;
};

void doDfsFromSpecialNode(Node* currentNode, Node* parent, const int sourceSpecialNodeId)
{
    auto& costSumsOnPathToSpecialNode = currentNode->costSumsOnPathToSpecialNode[sourceSpecialNodeId];
    if (parent)
    {
        // We can generate precisely the set of costs that our parent could, just by not including our cost in the subset ...
        costSumsOnPathToSpecialNode = parent->costSumsOnPathToSpecialNode[sourceSpecialNodeId];
        assert(costSumsOnPathToSpecialNode.size() == maxCostSum + 1);
        // ... but we can also generate "cost from source special node to parent" + "currentNode->cost", by
        // including currentNode in the subset!
        for (int costGeneratedByParent = maxCostSum - currentNode->cost; costGeneratedByParent >= 0; costGeneratedByParent--)
        {
            if (costSumsOnPathToSpecialNode[costGeneratedByParent].canBeGenerated)
            {
                costSumsOnPathToSpecialNode[costGeneratedByParent + currentNode->cost].canBeGenerated = true;
            }
        }
    }
    else
    {
        // The only ones we can generate are 0 (by not including currentNode) and currentNode->cost
        // (by including currentNode!)
        costSumsOnPathToSpecialNode.resize(maxCostSum + 1);
        costSumsOnPathToSpecialNode[0].canBeGenerated = true;
        costSumsOnPathToSpecialNode[currentNode->cost].canBeGenerated = true;
    }

    // Update nextLowestGeneratableCost for our CostInfo.
    int nextLowestGeneratableCost = -1;
    for (int cost = 0; cost <= maxCostSum; cost++)
    {
        costSumsOnPathToSpecialNode[cost].nextLowestGeneratableCost = nextLowestGeneratableCost;
        if (costSumsOnPathToSpecialNode[cost].canBeGenerated)
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

// Replace currentPVValue with newPVValue if the latter is "better" 
// than the former.
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

PVValue findBestPVValueFromTwoCostInfos(const vector<CostInfo>& costInfo1, const vector<CostInfo>& costInfo2, const int costSumLimit)
{
    PVValue result;

    auto findLargestGeneratableCost = [costSumLimit](const vector<CostInfo>& costInfo)
    {
        int largestCost = costSumLimit;
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

    // For each costIn1, nextCostIn2AtLeastCostIn1 will be the smallest
    // generatable cost in costInfo2 that is >= costInfo1, or equal to
    // nextCostIn2LessThanCostIn1 if there is no such cost in costInfo2.
    int nextCostIn2AtLeastCostIn1 = findLargestGeneratableCost(costInfo2);
    // For each costIn1, nextCostIn2LessThanCostIn1 will be the largest
    // generatable cost in costInfo2 that is < costInfo1, or equal to
    // nextCostIn2AtLeastCostIn1 if there is no such cost.
    int nextCostIn2LessThanCostIn1 = findLargestGeneratableCost(costInfo2);

    // Iterate over all generatable costIn1.
    while (costIn1 != -1)
    {
        // Update nextCostIn2LessThanCostIn1 and nextCostIn2AtLeastCostIn1 so that they
        // maintain the invariants described above.
        //
        while (nextCostIn2LessThanCostIn1 != -1 && nextCostIn2LessThanCostIn1 >= costIn1)
            nextCostIn2LessThanCostIn1 = costInfo2[nextCostIn2LessThanCostIn1].nextLowestGeneratableCost;
        while (nextCostIn2AtLeastCostIn1 != -1 && costInfo2[nextCostIn2AtLeastCostIn1].nextLowestGeneratableCost != -1 && costInfo2[nextCostIn2AtLeastCostIn1].nextLowestGeneratableCost >= costIn1)
            nextCostIn2AtLeastCostIn1 = costInfo2[nextCostIn2AtLeastCostIn1].nextLowestGeneratableCost;

        // By construction, nextLowestGeneratableCost and nextCostIn2AtLeastCostIn1 are the two
        // generatable costs in costInfo2 that are closest to costIn1, so will provide the
        // minimum difference from costIn1.
        incorporateNewCostPair(costIn1, nextCostIn2LessThanCostIn1);
        incorporateNewCostPair(costIn1, nextCostIn2AtLeastCostIn1);

        costIn1 = costInfo1[costIn1].nextLowestGeneratableCost;
    }

    return result;
}

int findBestPVValueForQuery(const Node* sourceNode, const Node* destNode, const int queryCostSumLimit, const vector<Node*>& specialNodes)
{
    PVValue result;

    // Just find the best PVValue over all possible pivots - there are at most 10 of them.
    for (int specialNodeIndex = 0; specialNodeIndex < specialNodes.size(); specialNodeIndex++)
    {
        const auto resultWithThisPivot = findBestPVValueFromTwoCostInfos(sourceNode->costSumsOnPathToSpecialNode[specialNodeIndex], 
                                                                         destNode->costSumsOnPathToSpecialNode[specialNodeIndex],
                                                                         queryCostSumLimit);

        updatePVValue(result, resultWithThisPivot);
    }
    return result.maxSumForMinDiff;
}

// Construct costSumsOnPathToSpecialNode[0 ... specialNodes.size() - 1][0 ... maxCostSum] for all nodes.
void buildLookupTable(vector<Node>& nodes, const vector<Node*>& specialNodes)
{
    for (auto& node : nodes)
    {
        node.costSumsOnPathToSpecialNode.resize(specialNodes.size());
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

    buildLookupTable(nodes, specialNodes);

    for (int q = 0; q < numQueries; q++)
    {
        const Node* sourceNode = &(nodes[read<int>() - 1]);
        const Node* destNode = &(nodes[read<int>() - 1]);
        const int queryCostSumLimit = read<int>();

        const auto bestPVValueForQuery = findBestPVValueForQuery(sourceNode, destNode, queryCostSumLimit, specialNodes);
        cout << bestPVValueForQuery << endl;
    }
    assert(cin);
}


