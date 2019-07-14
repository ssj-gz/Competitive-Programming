#include <iostream>
#include <vector>
#include <map>

#include <cassert>

#include <sys/time.h>

using namespace std;

const int numTripletPermutations = 6;

class ModNum
{
    public:
        ModNum(int64_t n = 0)
            : m_n{n}
        {
        }
        ModNum& operator+=(const ModNum& other)
        {
            m_n = (m_n + other.m_n) % modulus;
            return *this;
        }
        ModNum& operator-=(const ModNum& other)
        {
            m_n += modulus;
            assert(m_n >= other.m_n);
            m_n = (m_n - other.m_n) % modulus;
            return *this;
        }
        ModNum& operator*=(const ModNum& other)
        {
            m_n = (m_n * other.m_n) % modulus;
            return *this;
        }
        ModNum operator++(int)
        {
            m_n += 1;
            return *this;
        }
        int64_t value() const { return m_n; };

        static const int64_t modulus = 1'000'000'007ULL;
    private:
        int64_t m_n;
};

ModNum operator+(const ModNum& lhs, const ModNum& rhs)
{
    ModNum result(lhs);
    result += rhs;
    return result;
}
ModNum operator-(const ModNum& lhs, const ModNum& rhs)
{
    ModNum result(lhs);
    result -= rhs;
    return result;
}

ModNum operator*(const ModNum& lhs, const ModNum& rhs)
{
    ModNum result(lhs);
    result *= rhs;
    return result;
}

ostream& operator<<(ostream& os, const ModNum& toPrint)
{
    os << toPrint.value();
    return os;
}

bool operator==(const ModNum& lhs, const ModNum& rhs)
{
    return lhs.value() == rhs.value();
}


ModNum quickPower(long n, int m)
{
    // Raise n to the m mod modulus using as few multiplications as 
    // we can e.g. n ^ 8 ==  (((n^2)^2)^2).
    ModNum result = 1;
    int power = 0;
    while (m > 0)
    {
        if (m & 1)
        {
            ModNum subResult = n;
            for (int i = 0; i < power; i++)
            {
                subResult *= subResult;
            }
            result *= subResult;
        }
        m >>= 1;
        power++;
    }
    return result;
}

const ModNum inverseOf6 = quickPower(6, ModNum::modulus - 2);

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
    int64_t numPairsWithHeightViaDifferentChildren = 0;
    int64_t numTypeAAtCurrentNodeForHeight = 0;
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
                        cout << " Found  triple: " << node.id << ", " << node1->id << ", " << node2->id << " (distance: " << distance << ")" << endl;
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

    map<int, int64_t> numTypeAAtCurrentNodeForHeight;

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

            assert (descendentHeight > height);

            cout << " solveOptimisedAux currentNode: " << currentNode->id << " descendentHeight: " << descendentHeight << " heightInfo.numWithHeight: " << heightInfo.numWithHeight << " otherHeightInfo.numWithHeight: " << otherHeightInfo.numWithHeight << " after child: " << child->id << " numPairsWithHeightViaDifferentChildren:" << endl;
            // Triplets with currentNode as LCA of all pairs out of the three nodes.
            int newExtraDescendentHeight = -1;
            int knownDescendtHeight = -1;
            if (heightInfo.lastUpdatedAtNode == currentNode || heightInfo.lastUpdatedAtNode == nullptr)
            {
                assert(otherHeightInfo.lastUpdatedAtNode != currentNode);
                newExtraDescendentHeight = otherHeightInfo.numWithHeight;
                knownDescendtHeight = heightInfo.numWithHeight;

                otherHeightInfo.numPairsWithHeightViaDifferentChildren = heightInfo.numPairsWithHeightViaDifferentChildren;
                otherHeightInfo.numTypeAAtCurrentNodeForHeight = heightInfo.numTypeAAtCurrentNodeForHeight;
                otherHeightInfo.numWithHeight = heightInfo.numWithHeight;
            }
            else
            {
                assert(heightInfo.lastUpdatedAtNode != currentNode);
                cout << " currentNode: " << currentNode->id << " otherHeightInfo.lastUpdatedAtNode: " << (otherHeightInfo.lastUpdatedAtNode == nullptr ? -1 : otherHeightInfo.lastUpdatedAtNode->id) << endl;
                if (!(otherHeightInfo.lastUpdatedAtNode == nullptr || otherHeightInfo.lastUpdatedAtNode == currentNode))
                {
                    otherHeightInfo.numPairsWithHeightViaDifferentChildren = 0;
                    otherHeightInfo.numTypeAAtCurrentNodeForHeight = 0;
                }
                newExtraDescendentHeight = heightInfo.numWithHeight;
                knownDescendtHeight = otherHeightInfo.numWithHeight;
            }
            if (newExtraDescendentHeight * otherHeightInfo.numPairsWithHeightViaDifferentChildren > 0)
            {
                cout << " found threeway: adding: " << newExtraDescendentHeight * otherHeightInfo.numPairsWithHeightViaDifferentChildren << endl;
                numTriangles += newExtraDescendentHeight * otherHeightInfo.numPairsWithHeightViaDifferentChildren * numTripletPermutations;
            }



            cout << " currentNode: " << currentNode->id << " descendentHeight: " << descendentHeight << " numPairsWithHeightViaDifferentChildren: " << otherHeightInfo.numPairsWithHeightViaDifferentChildren << " newExtraDescendentHeight: " << newExtraDescendentHeight << endl;
            otherHeightInfo.numPairsWithHeightViaDifferentChildren += newExtraDescendentHeight * knownDescendtHeight;
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
                numTypeAAtCurrentNodeForHeight[descendentHeight] += numNewTriangles;
                otherHeightInfo.numTypeAAtCurrentNodeForHeight += numNewTriangles;
                cout << " currentNode: " << currentNode << " numTypeAAtCurrentNodeForHeight: " << descendentHeight << endl;
            }

            otherHeightInfo.numWithHeight += newExtraDescendentHeight;
            otherHeightInfo.lastUpdatedAtNode = currentNode;
            assert(numTypeAAtCurrentNodeForHeight[descendentHeight] == otherHeightInfo.numTypeAAtCurrentNodeForHeight);
        }
        ancestors.pop_back();
    }

    if (currentNode->hasPerson)
    {
        infoForDescendentHeight[height].numWithHeight++;
        infoForDescendentHeight[height].lastUpdatedAtNode = currentNode;
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

        const int numNodes = 1 + rand() % 10;
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
