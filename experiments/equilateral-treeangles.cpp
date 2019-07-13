#include <iostream>
#include <vector>
#include <map>

#include <cassert>

#include <sys/time.h>

using namespace std;

struct Node
{
    vector<Node*> neighbours;
    int id = -1; // 1-relative - handy for debugging.
    bool hasPerson = false;

    int index = -1; // 0-relative.

    bool visitedInBruteForceDFS = false;
    int dbgHeightInOptimisedDFS = -1; // TODO - remove
};

struct HeightInfo
{
    int numWithHeight = 0;
    int numPairsWithHeightViaDifferentChildren = 0;
    Node* lastUpdatedAtNode = nullptr;
};

void distDFS(const int rootNodeIndex, const Node* currentNode, const Node* parentNode, int height, vector<vector<int>>& distanceBetweenNodes)
{
    distanceBetweenNodes[rootNodeIndex][currentNode->index] = height;
    for (auto childNode : currentNode->neighbours)
    {
        if (childNode == parentNode)
            continue;

        distDFS(rootNodeIndex, childNode, currentNode, height + 1, distanceBetweenNodes);
    }
}

int64_t solveBruteForce(const vector<Node>& nodes)
{
    int64_t result = 0;

    const int numNodes = nodes.size();
    vector<vector<int>> distanceBetweenNodes(numNodes, vector<int>(numNodes, -1));
    for (const auto& rootNode : nodes)
    {
        distDFS(rootNode.index, &rootNode, nullptr, 0, distanceBetweenNodes);
    }
    for (int i = 0; i < numNodes; i++)
    {
        for (int j = 0; j < numNodes; j++)
        {
            assert(distanceBetweenNodes[i][j] != -1);
            assert(distanceBetweenNodes[i][j] == distanceBetweenNodes[j][i]);
            assert(distanceBetweenNodes[i][j] < numNodes);
        }
    }
    for (const auto& node : nodes)
    {
        if (!node.hasPerson)
            continue;
        vector<vector<const Node*>> nodesAtDistance(numNodes);
        for (int i = 0; i < numNodes; i++)
        {
            nodesAtDistance[distanceBetweenNodes[node.index][i]].push_back(&(nodes[i]));
        }
        for (int distance = 1; distance < numNodes; distance++)
        {
            const auto& nd = nodesAtDistance[distance];

            for (const auto node1 : nd)
            {
                if (!node1->hasPerson)
                    continue;
                for (const auto node2 : nd)
                {
                    if (!node2->hasPerson)
                        continue;
                    assert(distanceBetweenNodes[node.index][node1->index] == distance);
                    assert(distanceBetweenNodes[node.index][node2->index] == distance);

                    if (distanceBetweenNodes[node1->index][node2->index] == distance)
                    {
                        cout << " Found  triple: " << node.id << ", " << node1->id << ", " << node2->id << endl;
                        result++;
                    }
                }
            }
        }
    }

    return result;
}

void dbgCountHeights(Node* currentNode, Node* parentNode, int height, vector<int>& dbgNumAncestorsWithHeight)
{
    assert(height >= 0 && height < dbgNumAncestorsWithHeight.size());
    if (currentNode->hasPerson)
        dbgNumAncestorsWithHeight[height]++;
    //cout << "dbgCountHeights currentNode: " << (currentNode->id) << endl;
    for (auto child : currentNode->neighbours)
    {
        //cout << "wee" << endl;
        if (child == parentNode)
            continue;
        //cout << " child: " << child->id << endl;

        dbgCountHeights(child, currentNode, height + 1, dbgNumAncestorsWithHeight);
    }
}

int numNodes = 0;
 
map<int, HeightInfo> solveOptimisedAux(Node* currentNode, Node* parentNode, int height, vector<Node*>& ancestors, int64_t& numTriangles)
{
    assert(currentNode->dbgHeightInOptimisedDFS == -1);
    currentNode->dbgHeightInOptimisedDFS = height;
    map<int, HeightInfo> infoForDescendentHeight;

    const int numTripletPermutations = 6;

    if (currentNode->hasPerson)
    {
        infoForDescendentHeight[height].numWithHeight++;
    }

    auto nChoose2 = [](const int64_t n)
    {
        return (n * (n - 1)) / 2;
    };

    map<int, HeightInfo> localHeightInfo;

    for (auto child : currentNode->neighbours)
    {
        if (child == parentNode)
            continue;

        // TODO - the rest of this XD
        ancestors.push_back(currentNode);
        auto infoForChildDescendentHeight = solveOptimisedAux(child, currentNode, height + 1, ancestors, numTriangles);
        if (infoForChildDescendentHeight.size() < infoForDescendentHeight.size())
        {
            swap(infoForDescendentHeight, infoForChildDescendentHeight);
        }

        for (auto descendentHeightPair : infoForChildDescendentHeight)
        {
            const int descendentHeight = descendentHeightPair.first;

            auto& heightInfo = infoForChildDescendentHeight[descendentHeight];
            auto& otherHeightInfo = infoForDescendentHeight[descendentHeight];

            if (descendentHeight > height)
            {

                cout << " solveOptimisedAux currentNode: " << currentNode->id << " descendentHeight: " << descendentHeight << " heightInfo.numWithHeight: " << heightInfo.numWithHeight << " otherHeightInfo.numWithHeight: " << otherHeightInfo.numWithHeight << " after child: " << child->id << " numPairsWithHeightViaDifferentChildren:" << localHeightInfo[descendentHeight].numPairsWithHeightViaDifferentChildren << endl;
                // Triplets with currentNode as LCA of all pairs out of the three nodes.
                int newExtraDescendentHeight = -1;
                int knownDescendtHeight = -1;
                if (heightInfo.lastUpdatedAtNode == currentNode)
                {
                    assert(otherHeightInfo.lastUpdatedAtNode != currentNode);
                    newExtraDescendentHeight = otherHeightInfo.numWithHeight;
                    knownDescendtHeight = heightInfo.numWithHeight;
                    cout << " found threeways: " << otherHeightInfo.numWithHeight * localHeightInfo[descendentHeight].numPairsWithHeightViaDifferentChildren << endl;
                }
                else
                {
                    assert(heightInfo.lastUpdatedAtNode != currentNode);
                    newExtraDescendentHeight = heightInfo.numWithHeight;
                    knownDescendtHeight = otherHeightInfo.numWithHeight;
                }
                if (newExtraDescendentHeight * localHeightInfo[descendentHeight].numPairsWithHeightViaDifferentChildren > 0)
                {
                    cout << " found threeway: adding: " << newExtraDescendentHeight * localHeightInfo[descendentHeight].numPairsWithHeightViaDifferentChildren << endl;
                    numTriangles += newExtraDescendentHeight * localHeightInfo[descendentHeight].numPairsWithHeightViaDifferentChildren * numTripletPermutations;
                }



                cout << " currentNode: " << currentNode->id << " descendentHeight: " << descendentHeight << " numPairsWithHeightViaDifferentChildren: " << localHeightInfo[descendentHeight].numPairsWithHeightViaDifferentChildren << " newExtraDescendentHeight: " << newExtraDescendentHeight << endl;
                localHeightInfo[descendentHeight].numPairsWithHeightViaDifferentChildren += heightInfo.numWithHeight * otherHeightInfo.numWithHeight;
                if (parentNode != nullptr)
                {
                    vector<int> dbgNumDescendantsWithHeight(::numNodes, 0);
                    const int requiredNonDescendantDist = (descendentHeight - height);
                    dbgCountHeights(parentNode, currentNode, 1, dbgNumDescendantsWithHeight);
                    const int numNewTriangles = knownDescendtHeight * newExtraDescendentHeight * dbgNumDescendantsWithHeight[requiredNonDescendantDist];
                    cout << " currentNode: " << currentNode->id << " num non-ancestors with dist: " << requiredNonDescendantDist << ": " << dbgNumDescendantsWithHeight[requiredNonDescendantDist] << endl;
                    if (numNewTriangles > 0)
                    {
                        cout << " found double: adding: " << numNewTriangles << endl;
                    }
                    numTriangles += numNewTriangles * numTripletPermutations;
                }
            }
            otherHeightInfo.numWithHeight += heightInfo.numWithHeight;
            otherHeightInfo.lastUpdatedAtNode = currentNode;
        }
        ancestors.pop_back();
    }
    return infoForDescendentHeight;
}

int64_t solveOptimised(vector<Node>& nodes)
{
    int64_t result = 0;

    const int numNodes = nodes.size();
    ::numNodes = numNodes;

    Node* rootNode = &(nodes.front());
    vector<Node*> ancestors;
    solveOptimisedAux(rootNode, nullptr, 0, ancestors, result);

    return result;
}

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int numNodes = 1 + rand() % 100;
        cout << numNodes << endl;

        for (int i = 0; i < numNodes - 1; i++)
        {
            cout << (i + 2) << " " << ((rand() % (i + 1) + 1)) << endl;
        }
        for (int i = 0; i < numNodes; i++)
        {
            cout << (rand() % 2) << endl;
        }
        return 0;
    }
    int numNodes;
    cin >> numNodes;

    vector<Node> nodes(numNodes);

    for (int i = 0; i < numNodes - 1; i++)
    {
        int u;
        cin >> u;
        int v;
        cin >> v;

        // Make 0-relative.
        u--;
        v--;

        nodes[u].neighbours.push_back(&(nodes[v]));
        nodes[v].neighbours.push_back(&(nodes[u]));
    }
    for (int i = 0; i < numNodes; i++)
    {
        int hasPerson;
        cin >> hasPerson;

        nodes[i].hasPerson = (hasPerson == 1);

        nodes[i].index = i;
        nodes[i].id = i + 1;
    }
    assert(cin);

    const auto solutionBruteForce = solveBruteForce(nodes);
    const auto solutionOptimised = solveOptimised(nodes);
    cout << "solutionBruteForce: " << solutionBruteForce << endl;
    cout << "solutionOptimised: " << solutionOptimised  << " (" << (solutionBruteForce / 6) << " basic triangles)"<< endl;
    assert(solutionOptimised == solutionBruteForce);
}
