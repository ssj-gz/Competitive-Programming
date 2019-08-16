// Simon St James (ssjgz) - 2019-XX-XX
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>

#include <cassert>

#include <sys/time.h>

using namespace std;

    template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

#if 0
vector<int64_t> solveBruteForce()
{
    vector<int64_t> result;

    return result;
}
#endif

const auto maxCost = 1000;

struct CostInfo
{
    bool canBeGenerated = false;
    int nextLowestGeneratableCost = -1;
};

struct Node
{
    vector<Node*> neighbours;
    int id = -1;

    int cost = 0;

    int specialNodeIndex = -1;

    vector<vector<CostInfo>> generatableCostsOnPathToSpecialNode;
};

void doDfsFromSpecialNode(Node* currentNode, Node* parent, const int sourceSpecialNodeId)
{
    cout << "doDfsFromSpecialNode: currentNode: " << currentNode->id << " sourceSpecialNodeId: " << sourceSpecialNodeId << endl;
    auto& generatableCostsOnPathToSpecialNode = currentNode->generatableCostsOnPathToSpecialNode[sourceSpecialNodeId];
    if (parent)
    {
        generatableCostsOnPathToSpecialNode = parent->generatableCostsOnPathToSpecialNode[sourceSpecialNodeId];
        assert(generatableCostsOnPathToSpecialNode.size() == maxCost + 1);
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
        generatableCostsOnPathToSpecialNode.resize(maxCost + 1);
        generatableCostsOnPathToSpecialNode[0].canBeGenerated = true;
        generatableCostsOnPathToSpecialNode[currentNode->cost].canBeGenerated = true;
    }

    cout << "currentNode: " << currentNode->id << endl;
    cout << "Generatable costs from sourceSpecialNodeId: " << sourceSpecialNodeId << endl;
    for (int cost = 0; cost <= maxCost; cost++)
    {
        if (generatableCostsOnPathToSpecialNode[cost].canBeGenerated)
            cout << " " << cost << endl;
    }

    int nextLowestGeneratableCost = -1;
    for (int cost = 0; cost <= maxCost; cost++)
    {
        generatableCostsOnPathToSpecialNode[cost].nextLowestGeneratableCost = nextLowestGeneratableCost;
        if (generatableCostsOnPathToSpecialNode[cost].canBeGenerated)
            nextLowestGeneratableCost = cost;
    }

    for (auto child : currentNode->neighbours)
    {
        if (child == parent)
            continue;

        doDfsFromSpecialNode(child, currentNode, sourceSpecialNodeId);
    }
}

struct PVValue
{
    int minX = -1;
    int valueWithMinX = -1;
};

PVValue findPVValue(const vector<CostInfo>& costInfo1, const vector<CostInfo>& costInfo2, const int costLimit)
{
    PVValue result;

    auto findLargestGeneratableCost = [costLimit](const vector<CostInfo>& costInfo)
    {
        int largestCost = costLimit;
        if (!costInfo[largestCost].canBeGenerated)
            largestCost = costInfo[largestCost].nextLowestGeneratableCost;

        assert(largestCost != -1);

        return largestCost;
    };

    auto incorporateNewCostPair = [&result](const int costIn1, const int costIn2)
    {
        const int newDifference = abs(costIn1 - costIn2);
        assert(newDifference >= 0);
        if (result.minX == -1 || result.minX > newDifference)
        {
            result.minX = newDifference;
            result.valueWithMinX = -1;
        }
        if (newDifference == result.minX)
        {
            const int newSum = costIn1 + costIn2;
            result.valueWithMinX = max(result.valueWithMinX, newSum);
        }
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
        cout << " costIn1: " << costIn1 << " nextCostIn2LessThanCostIn1: " << nextCostIn2LessThanCostIn1 << " nextCostIn2AtLeastCostIn1: " << nextCostIn2AtLeastCostIn1 << endl;

        incorporateNewCostPair(costIn1, nextCostIn2LessThanCostIn1);
        incorporateNewCostPair(costIn1, nextCostIn2AtLeastCostIn1);

        costIn1 = costInfo1[costIn1].nextLowestGeneratableCost;
    }
    

    return result;
}

int findBestPVValueForQuery(const Node* sourceNode, const Node* destNode, const int costLimit, const int numSpecialNodes)
{
    cout << " query: sourceNode: " << sourceNode->id << " destNode: " << destNode->id << " costLimit: " << costLimit << endl;
    PVValue result;

    auto incorporateNewPVValue = [&result](const PVValue& newPVValue)
    {
        if (result.minX == -1 || result.minX > newPVValue.minX)
        {
            result.minX = newPVValue.minX;
            result.valueWithMinX = -1;
        }
        if (newPVValue.minX == result.minX)
        {
            result.valueWithMinX = max(result.valueWithMinX, newPVValue.valueWithMinX);
        }
    };

    for (int specialNodeIndex = 0; specialNodeIndex < numSpecialNodes; specialNodeIndex++)
    {
        const auto resultWithThisPivot = findPVValue(sourceNode->generatableCostsOnPathToSpecialNode[specialNodeIndex], 
                                                     destNode->generatableCostsOnPathToSpecialNode[specialNodeIndex],
                                                     costLimit);

        incorporateNewPVValue(resultWithThisPivot);
    }
    return result.valueWithMinX;
}

void buildLookupTables(vector<Node>& nodes, const vector<Node*>& specialNodes)
{
    for (auto& node : nodes)
    {
        node.generatableCostsOnPathToSpecialNode.resize(specialNodes.size());
    }

    for (auto specialNode : specialNodes)
    {
        doDfsFromSpecialNode(specialNode, nullptr, specialNode->specialNodeIndex);
    }
    for (auto& node : nodes)
    {
        assert(node.generatableCostsOnPathToSpecialNode.size() == specialNodes.size());
        for (int i = 0; i < specialNodes.size(); i++)
        {
            assert(node.generatableCostsOnPathToSpecialNode[i].size() == maxCost + 1);
        }
    }
}

vector<int64_t> solveOptimised()
{
    vector<int64_t> result;

    return result;
}


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        // TODO - generate randomised test.
        return 0;
    }

    const int numNodes = read<int>();
    const int numSpecialNodes = read<int>();
    const int numQueries = read<int>();

    vector<Node> nodes(numNodes);
    for (int i = 0; i < numNodes; i++)
    {
        nodes[i].id = i + 1;
    }

    for (int i = 0; i < numNodes - 1; i++)
    {
        const int v = read<int>() - 1;

        nodes[v].neighbours.push_back(&(nodes[i + 1]));
        nodes[i + 1].neighbours.push_back(&(nodes[v]));

        cout << "edge between: " << nodes[i + 1].id << " and " << nodes[v].id << endl;
    }

    for (int i = 0; i < numNodes; i++)
    {
        nodes[i].cost = read<int>();
    }

    vector<Node*> specialNodes;

    for (int i = 0; i < numSpecialNodes; i++)
    {
        specialNodes.push_back(&(nodes[read<int>() - 1]));
        specialNodes.back()->specialNodeIndex = i;
    }

    buildLookupTables(nodes, specialNodes);

    for (int q = 0; q < numQueries; q++)
    {
        Node* sourceNode = &(nodes[read<int>() - 1]);
        Node* destNode = &(nodes[read<int>() - 1]);
        int maxCost = read<int>() - 1;

        cout << findBestPVValueForQuery(sourceNode, destNode, maxCost, numSpecialNodes) << endl;
    }

#ifdef BRUTE_FORCE
#if 0
    const auto solutionBruteForce = solveBruteForce();
    cout << "solutionBruteForce: " << solutionBruteForce << endl;
#endif
#if 0
    const auto solutionOptimised = solveOptimised();
    cout << "solutionOptimised: " << solutionOptimised << endl;

    assert(solutionOptimised == solutionBruteForce);
#endif
#else
    const auto solutionOptimised = solveOptimised();
    cout << solutionOptimised << endl;
#endif

}


