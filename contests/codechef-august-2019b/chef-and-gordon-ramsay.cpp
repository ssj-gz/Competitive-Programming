// Simon St James (ssjgz) - 2019-08-06

// O(N^2) solution - no chance of getting AC, but let's check correctness
// and scrounge up some points at least :)

//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <array>
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

struct Node
{
    vector<Node*> neighbours;
    int id = -1;
};

class Triple
{
    public:
        Triple(int a1, int a2, int a3)
            : m_elements{{a1, a2, a3}}
        {
        }
        int operator[](int i) const
        {
            return m_elements[i];
        }
    private:
        std::array<int, 3> m_elements;
};

ostream& operator<<(ostream& os, const Triple& triple)
{
    cout << "(" << triple[0] << ", " << triple[1] << ", " << triple[2] << ")";
    return os;
}

bool operator<(const Triple& lhs, const Triple& rhs)
{
    if (lhs[0] != rhs[0])
        return lhs[0] < rhs[0];
    if (lhs[1] != rhs[1])
        return lhs[1] < rhs[1];
    return lhs[2] < rhs[2];
}


void solutionBruteForceAux(Node* startNode, Node* currentNode, Node* parentNode, const array<int, 3>& P, vector<Node*> nodesSoFar, vector<Triple>& result)
{

    //cout << "startNode: " << startNode->id << " currentNode: " << currentNode->id << endl;

    auto havePOrdering = [&P](Node* node1, Node* node2, int pIndex1, int pIndex2) -> bool
    {
        return (((node1->id < node2->id) && (P[pIndex1] < P[pIndex2])) ||
                ((node1->id > node2->id) && (P[pIndex1] > P[pIndex2])));
    };

    if (havePOrdering(startNode, currentNode, 0, 2))
    {
        //cout << " have POrdering" << endl;
        for (auto nodeinPath : nodesSoFar)
        {
            //cout << "  nodeinPath: " << nodeinPath->id << endl;
            if (havePOrdering(startNode, nodeinPath, 0, 1) && havePOrdering(nodeinPath, currentNode, 1, 2))
            {
                //cout << "Found! (" << startNode->id << ", " << nodeinPath->id << ", " << currentNode->id << ")" << endl;
                result.emplace_back(startNode->id, nodeinPath->id, currentNode->id);
            }
        }

    }


    if (currentNode != startNode)
        nodesSoFar.push_back(currentNode);

    for (auto child : currentNode->neighbours)
    {
        if (child == parentNode)
            continue;
        solutionBruteForceAux(startNode, child, currentNode, P, nodesSoFar, result);
    }

    if (currentNode != startNode)
        nodesSoFar.pop_back();
}


vector<Triple> solveBruteForce(vector<Node>& nodes, const array<int, 3>& P)
{
    vector<Triple> result;

    for (auto& node : nodes)
    {
        vector<Node*> nodesSoFar;
        solutionBruteForceAux(&node, &node, nullptr, P, nodesSoFar, result);
    }

    return result;
}

// Typical SegmentTree - you can find similar implementations all over the place :)
class SegmentTree
{
    public:
        SegmentTree(int numElements)
            : m_size{numElements},
              m_numElements{2 * numElements},
              m_elements(m_numElements + 1)
            {
            }
        int total() const
        {
            return m_total;
        }
        int size() const
        {
            return m_size;
        }
        // Find the number in the given range (inclusive) in O(log2(numElements)).
        int numInRange(int start, int end) const
        {
            start++; // Make 1-relative.  start and end are inclusive.
            end++;
            auto sum = 0;
            auto elements = m_elements.data();
            while(end > 0)
            {
                sum += elements[end];
                end -= (end & (end*-1));
            }
            start--;
            while(start > 0)
            {
                sum -= elements[start];
                start -= (start & (start*-1));
            }
            return sum;
        }

        // Add "value" to the current value at pos in O(log2(numElements)).
        void addValueAt(int value, int pos)
        {
            const auto n = m_numElements;
            auto elements = m_elements.data();
            pos++; // Make 1-relative.
            while(pos <= n)
            {
                elements[pos] += value;
                assert(elements[pos] >= 0);
                pos += (pos & (pos * -1));
            }

            m_total += value;
        }

    private:
        int m_size;
        int m_numElements;
        int m_total = 0;
        vector<int> m_elements;

};

void solutionOptimisedAux(Node* startNode, Node* currentNode, Node* parentNode, const array<int, 3>& P, SegmentTree& segmentTree, int64_t& result)
{

    //cout << "startNode: " << startNode->id << " currentNode: " << currentNode->id << endl;

    auto havePOrdering = [&P](Node* node1, Node* node2, int pIndex1, int pIndex2) -> bool
    {
        return (((node1->id < node2->id) && (P[pIndex1] < P[pIndex2])) ||
                ((node1->id > node2->id) && (P[pIndex1] > P[pIndex2])));
    };

    if (havePOrdering(startNode, currentNode, 0, 2))
    {
        //cout << " have POrdering" << endl;
        const int leftId = min(startNode->id, currentNode->id);
        const int rightId = max(startNode->id, currentNode->id);
        if ((P[0] < P[1] && P[1] < P[2]) ||
                (P[0] > P[1] && P[1] > P[2]))
        {
            result += segmentTree.numInRange(leftId, rightId);
        }
        else if (P[1] < P[0] && P[1] < P[2])
        {
            result += segmentTree.numInRange(0, leftId);
        }
        else if (P[1] > P[0] && P[1] > P[2])
        {
            result += segmentTree.numInRange(rightId, segmentTree.size());
        }
    }

    if (currentNode != startNode)
        segmentTree.addValueAt(1, currentNode->id);

    for (auto child : currentNode->neighbours)
    {
        if (child == parentNode)
            continue;
        solutionOptimisedAux(startNode, child, currentNode, P, segmentTree, result);
    }

    if (currentNode != startNode)
        segmentTree.addValueAt(-1, currentNode->id);
}


int64_t solveOptimised(vector<Node>& nodes, const array<int, 3>& P)
{
    int64_t result = 0;

    for (auto& node : nodes)
    {
        SegmentTree segmentTree(nodes.size() + 1);
        solutionOptimisedAux(&node, &node, nullptr, P, segmentTree, result);
    }

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

        const int maxN = 1000;
        const int N = rand() % maxN + 1;

        vector<int> P = {1, 2, 3};
        random_shuffle(P.begin(), P.end());

        cout << 1 << endl;
        cout << N << endl;

        cout << P[0] << " " << P[1] << " " << P[2] << endl;

        vector<int> nodeIds;
        for (int i = 1; i <= N; i++)
        {
            nodeIds.push_back(i);
        }
        random_shuffle(nodeIds.begin(), nodeIds.end());

        vector<int> usedNodeIds = { nodeIds.back() };
        nodeIds.pop_back();

        for (int i = 0; i < N - 1; i++)
        {
            const int newNodeId = nodeIds.back();
            const int oldNodeId = usedNodeIds[rand() % usedNodeIds.size()];

            cout << newNodeId << " " << oldNodeId << endl;

            usedNodeIds.push_back(newNodeId);
            nodeIds.pop_back();
        }

        return 0;
    }

    const int T = read<int>();
    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();

        vector<Node> nodes(N);
        for (int i = 0; i < N; i++)
        {
            nodes[i].id = i + 1;
        }

        array<int, 3> P;
        for (auto& p : P)
        {
            p = read<int>();
        }

        for (int i = 0; i < N - 1; i++)
        {
            const int u = read<int>() - 1;
            const int v = read<int>() - 1;

            nodes[u].neighbours.push_back(&(nodes[v]));
            nodes[v].neighbours.push_back(&(nodes[u]));
        }
#ifdef BRUTE_FORCE
        const auto solutionBruteForce = solveBruteForce(nodes, P).size();
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
        const auto solutionOptimised = solveOptimised(nodes, P);
        cout << "solutionOptimised: " << solutionOptimised << endl;

        assert(solutionOptimised == solutionBruteForce);
#else
        const auto solutionOptimised = solveOptimised(nodes, P);
        cout << solutionOptimised << endl;
#endif
    }


}


