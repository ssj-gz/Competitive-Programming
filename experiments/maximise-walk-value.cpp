// Simon St James (ssjgz) - 2019-08-16
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <algorithm>

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
    cout << "doDfsFromSpecialNode: currentNode: " << currentNode->id << " sourceSpecialNodeId: " << sourceSpecialNodeId << " generatableCostsOnPathToSpecialNode.size(): " << currentNode->generatableCostsOnPathToSpecialNode.size() << endl;
    assert(sourceSpecialNodeId < currentNode->generatableCostsOnPathToSpecialNode.size());
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

    //cout << "currentNode: " << currentNode->id << endl;
    //cout << "Generatable costs from sourceSpecialNodeId: " << sourceSpecialNodeId << endl;
#if 0
    for (int cost = 0; cost <= maxCost; cost++)
    {
        if (generatableCostsOnPathToSpecialNode[cost].canBeGenerated)
            cout << " " << cost << endl;
    }
#endif

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

void updatePVValue(PVValue& currentPVValue, const PVValue& newPVValue)
{
    if (currentPVValue.minX == -1)
    {
        currentPVValue = newPVValue;
        return;
    }
    if (currentPVValue.minX < newPVValue.minX)
        return;
    if (currentPVValue.minX > newPVValue.minX)
    {
        currentPVValue = newPVValue;
    }
    else
    {
        assert(currentPVValue.minX == newPVValue.minX);
        currentPVValue.valueWithMinX = max(currentPVValue.valueWithMinX, newPVValue.valueWithMinX);
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
        //cout << " costIn1: " << costIn1 << " nextCostIn2LessThanCostIn1: " << nextCostIn2LessThanCostIn1 << " nextCostIn2AtLeastCostIn1: " << nextCostIn2AtLeastCostIn1 << endl;

        incorporateNewCostPair(costIn1, nextCostIn2LessThanCostIn1);
        incorporateNewCostPair(costIn1, nextCostIn2AtLeastCostIn1);

        costIn1 = costInfo1[costIn1].nextLowestGeneratableCost;
    }
    

    return result;
}

int findBestPVValueForQuery(const Node* sourceNode, const Node* destNode, const int costLimit, const vector<Node*>& specialNodes)
{
    //cout << " query: sourceNode: " << sourceNode->id << " destNode: " << destNode->id << " costLimit: " << costLimit << endl;
    PVValue result;

    for (int specialNodeIndex = 0; specialNodeIndex < specialNodes.size(); specialNodeIndex++)
    {
        //cout << " query: sourceNode: " << sourceNode->id << " destNode: " << destNode->id << " costLimit: " << costLimit << " specialNode: " << specialNodes[specialNodeIndex]->id  << endl;
        const auto resultWithThisPivot = findPVValue(sourceNode->generatableCostsOnPathToSpecialNode[specialNodeIndex], 
                                                     destNode->generatableCostsOnPathToSpecialNode[specialNodeIndex],
                                                     costLimit);

        updatePVValue(result, resultWithThisPivot);
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
#ifndef NDEBUG
    for (auto& node : nodes)
    {
        assert(node.generatableCostsOnPathToSpecialNode.size() == specialNodes.size());
        for (int i = 0; i < specialNodes.size(); i++)
        {
            assert(node.generatableCostsOnPathToSpecialNode[i].size() == maxCost + 1);
        }
    }
#endif
}

void findWeightListAlongPathAux(const Node* currentNode, const Node* parent, const Node* pathEnd, vector<int>& weightListSoFar, vector<int>& answer)
{
    weightListSoFar.push_back(currentNode->cost);
    if (currentNode == pathEnd)
        answer = weightListSoFar;

    for (auto child : currentNode->neighbours)
    {
        if (child == parent)
            continue;

        findWeightListAlongPathAux(child, currentNode, pathEnd, weightListSoFar, answer);
    }

    weightListSoFar.pop_back();
}

vector<int> findCostsGeneratableFromList(const vector<int>& costList)
{
    vector<bool> useCost(costList.size(), false);

    vector<int> results;

    while (true)
    {
        int cost = 0;
        for (int i = 0; i < costList.size(); i++)
        {
            if (useCost[i])
                cost += costList[i];
        }
        results.push_back(cost);

        int index = 0;
        while (index < costList.size() && useCost[index] == true)
        {
            useCost[index] = false;
            index++;
        }
        if (index == costList.size())
            break;
        useCost[index] = true;
    }

    sort(results.begin(), results.end());
    results.erase(unique(results.begin(), results.end()), results.end());

    return results;
}

vector<int> findWeightListAlongPath(const Node* pathBegin, const Node* pathEnd)
{
    vector<int> result;
    vector<int> weightListSoFar;

    findWeightListAlongPathAux(pathBegin, nullptr, pathEnd, weightListSoFar, result);

    return result;
}


int solveQueryBruteForce(const Node* sourceNode, const Node* destNode, const int costLimit, const vector<Node*>& specialNodes)
{
    PVValue result;
    for (auto specialNode : specialNodes)
    {
        const auto weightSumsGeneratedSourceToSpecial = findCostsGeneratableFromList(findWeightListAlongPath(sourceNode, specialNode)); 
        const auto weightSumsGeneratedSpecialToDest = findCostsGeneratableFromList(findWeightListAlongPath(specialNode, destNode)); 

        cout << "weightSumsGeneratedSourceToSpecial: " << endl;
        for (const auto x : weightSumsGeneratedSourceToSpecial)
            cout << x << " ";
        cout << endl;
        cout << "weightSumsGeneratedSpecialToDest: " << endl;
        for (const auto x : weightSumsGeneratedSpecialToDest)
            cout << x << " ";
        cout << endl;

        for (const auto cost1 : weightSumsGeneratedSourceToSpecial)
        {
            if (cost1 > costLimit)
                continue;
            for (const auto cost2 : weightSumsGeneratedSpecialToDest)
            {
                if (cost2 > costLimit)
                    continue;
                PVValue newPVValue = { abs(cost1 - cost2), cost1 + cost2 };
                updatePVValue(result, newPVValue);
            }
        }
    }

    return result.valueWithMinX;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        const int numNodes = rand() % 100 + 1;
        const int numSpecialNodes = rand() % numNodes + 1;
        const int numQueries = rand() % 100 + 1;
        //const int numQueries = 1;

        const int maxCostPerNode = rand() % 100 + 1;
        const int maxCostInQuery = rand() % 1000 + 1;

        cout << numNodes << " " << numSpecialNodes << " " << numQueries << endl;
        for (int i = 0; i < numNodes - 1; i++)
        {
            cout << ((rand() % (i + 1)) + 1) << " ";
        }
        cout << endl;

        vector<int> nodeIds;
        for (int i = 0; i < numNodes; i++)
        {
            nodeIds.push_back(i + 1);
            cout << ((rand() % maxCostPerNode)) << " ";
        }
        cout << endl;

        random_shuffle(nodeIds.begin(), nodeIds.end());

        for (int i = 0; i < numSpecialNodes; i++)
        {
            cout << (nodeIds[i]) << " ";
        }
        cout << endl;

        for (int i = 0; i < numQueries; i++)
        {
            const int nodeId1 = (rand() % numNodes) + 1;
            const int nodeId2 = (rand() % numNodes) + 1;
            const int costLimit = rand() % maxCostInQuery;

            cout << nodeId1 << " " << nodeId2 << " " << costLimit << endl; 
        }

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
        cout << "node cost: " << nodes[i].cost << endl;
    }

    vector<Node*> specialNodes;

    for (int i = 0; i < numSpecialNodes; i++)
    {
        specialNodes.push_back(&(nodes[read<int>() - 1]));
        specialNodes.back()->specialNodeIndex = i;
        cout << " special node: " << specialNodes.back()->id << endl;
    }

    buildLookupTables(nodes, specialNodes);

    for (int q = 0; q < numQueries; q++)
    {
        Node* sourceNode = &(nodes[read<int>() - 1]);
        Node* destNode = &(nodes[read<int>() - 1]);
        int maxCost = read<int>();
        assert(cin);

#ifdef BRUTE_FORCE
        const auto queryResultOptimised = findBestPVValueForQuery(sourceNode, destNode, maxCost, specialNodes);
        cout << "queryResultOptimised: " << queryResultOptimised << endl;
        const auto queryResultBruteForce = solveQueryBruteForce(sourceNode, destNode, maxCost, specialNodes);
        cout << "queryResultBruteForce: " << queryResultBruteForce << endl;

        assert(queryResultOptimised == queryResultBruteForce);
#else
        const auto queryResultOptimised = findBestPVValueForQuery(sourceNode, destNode, maxCost, specialNodes);
        cout << queryResultOptimised << endl;
#endif
    }
}


