// Simon St James (ssjgz) - 2019-08-06

#define SUBMISSION
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

#include <sys/resource.h>


using namespace std;

#define gc getchar_unlocked

// Lots of input to read, so use ultra-fast reader.
void scan_integer( int &x )
{
    int c = gc();
    x = 0;
    for( ; ((c<48 || c>57) && c != '-'); c = gc() );
    for( ;c>47 && c<58; c = gc() ) {
        x = (x << 1) + (x << 3) + c - 48;
    }
}


template <typename T>
T read()
{
    T toRead;
    scan_integer(toRead);
    assert(cin);
    return toRead;
}

struct Node
{
    vector<Node*> children;
    int id = -1;

    int numDescendentsLessThan = 0;
    int numDescendentsGreaterThan = 0;
    int numLessThanOnInitialVisitWithOriginalOrder = 0;
    int numGreaterThanOnInitialVisitWithOriginalOrder = 0;
};

void fixParentChild(Node* node, Node* parent)
{
    if (parent)
        node->children.erase(find(node->children.begin(), node->children.end(), parent));
    for (auto child : node->children)
        fixParentChild(child, node);
}

// Typical SegmentTree - you can find similar implementations all over the place :)
class SegmentTree
{
    public:
        SegmentTree() = default;
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
        int64_t numInRange(int start, int end) const
        {
            start++; // Make 1-relative.  start and end are inclusive.
            end++;
            int64_t sum = 0;
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
        int64_t numToRightOf(int pos)
        {
            return total() - numToLeftOf(pos);
            //return numInRange(pos + 1, size());
        }
        int64_t numToLeftOf(int pos)
        {
            return numInRange(0, pos - 1);
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
        vector<int64_t> m_elements;
};

void solveOptimisedAuxLCANoneOfA1A2A3(const vector<Node>& nodes, const std::array<int, 3>& P, int64_t& result)
{
    const bool isA2LessThanA1 = (P[1] < P[0]);
    const bool isA2LessThanA3 = (P[1] < P[2]);

    const int numNodes = nodes.size();

    for (const Node& a2 : nodes)
    {

        const auto totalNumNodesGreaterThan = (numNodes - a2.id);
        const auto totalNonDescendentNodesGreaterThan = totalNumNodesGreaterThan - a2.numDescendentsGreaterThan;

        const auto totalNumNodesLessThan = a2.id - 1;
        const auto totalNonDescendentNodesLessThan = totalNumNodesLessThan - a2.numDescendentsLessThan;

        const auto numA1 = isA2LessThanA1 ? totalNonDescendentNodesGreaterThan : totalNonDescendentNodesLessThan;

        if (isA2LessThanA3)
        {
            result += static_cast<int64_t>(numA1) * a2.numDescendentsGreaterThan;
        }
        else
        {
            result += static_cast<int64_t>(numA1) * a2.numDescendentsLessThan;
        }

    }

}

void solveOptimisedAuxLCAIsA2(Node* currentNode, SegmentTree& nodeTracker, const std::array<int, 3>& P, int64_t& result)
{
    const bool isA2LessThanA1 = (P[1] < P[0]);
    const bool isA2LessThanA3 = (P[1] < P[2]);

    const int nodeId = currentNode->id;

    // What if we are a2?
    const auto initialNumGreaterThan = nodeTracker.numToRightOf(nodeId);
    const auto initialNumLessThan = nodeTracker.total() - initialNumGreaterThan;
    int descendantsGreaterThanSoFar = 0;
    int descendantsLessThanSoFar = 0;

    //cout << " at node: " << currentNode->id << " initialNumGreaterThan: " << initialNumGreaterThan << endl;

    // What if we are a1 or a3?
    nodeTracker.addValueAt(1, nodeId);

    for (Node* childNode : currentNode->children)
    {

        solveOptimisedAuxLCAIsA2(childNode, nodeTracker, P, result);
        const int numLess = nodeTracker.numToLeftOf(nodeId);
        const int numGreater = nodeTracker.total() - numLess - 1;
        const int numOfThisChildGreaterThan = (numGreater - initialNumGreaterThan) - descendantsGreaterThanSoFar;
        const int numOfThisChildLessThan = (numLess - initialNumLessThan) - descendantsLessThanSoFar;
        //cout << "  at node: " << currentNode << " explored child: " << childNode->id << " numOfThisChildGreaterThan: " << numOfThisChildGreaterThan << " descendantsGreaterThanSoFar: " << descendantsGreaterThanSoFar << "  numOfThisChildLessThan: " << numOfThisChildLessThan << " descendantsLessThanSoFar: "<< descendantsLessThanSoFar << endl;

        //cout << " isA2LessThanA1: " << isA2LessThanA1 << " isA2LessThanA3: " << isA2LessThanA3 << endl;

        if (isA2LessThanA1 && isA2LessThanA3)
        {
            //cout << " glarp" << endl;
            result += static_cast<int64_t>(numOfThisChildGreaterThan) * descendantsGreaterThanSoFar;
        }
        else if (!isA2LessThanA1 && !isA2LessThanA3)
        {
            //cout << " glorp" << endl;
            result += static_cast<int64_t>(numOfThisChildLessThan) * descendantsLessThanSoFar;
        }
        else 
        {
            //cout << " lca is a2 node: " << currentNode->id << " adding " << (numOfThisChildLessThan * descendantsGreaterThanSoFar) << " to result" << endl;
            result += static_cast<int64_t>(numOfThisChildLessThan) * descendantsGreaterThanSoFar;
            result += static_cast<int64_t>(numOfThisChildGreaterThan) * descendantsLessThanSoFar;
        }

        descendantsGreaterThanSoFar += numOfThisChildGreaterThan;
        descendantsLessThanSoFar += numOfThisChildLessThan;
    }

    currentNode->numDescendentsLessThan = descendantsLessThanSoFar;
    currentNode->numDescendentsGreaterThan = descendantsGreaterThanSoFar;
    currentNode->numLessThanOnInitialVisitWithOriginalOrder = initialNumLessThan;
    currentNode->numGreaterThanOnInitialVisitWithOriginalOrder = initialNumGreaterThan;

}


int64_t solveOptimised2(vector<Node>& nodes, const array<int, 3>& Parray)
{
    int64_t result = 0;
    auto rootNode = &(nodes.front());
    const std::array<int, 3> P = { Parray[0], Parray[1], Parray[2] };
    const std::array<int, 3> reversedP = { Parray[2], Parray[1], Parray[0] };

    fixParentChild(rootNode, nullptr);

    const bool isPMonotonic = (P[2] > P[1] && P[1] > P[0]) || (P[2] < P[1] && P[1] < P[0]);


    SegmentTree nodeTracker(nodes.size() + 1);
    solveOptimisedAuxLCAIsA2(rootNode, nodeTracker, P, result);

    if (!isPMonotonic)
    {
        solveOptimisedAuxLCANoneOfA1A2A3(nodes, P, result);
    }
    else
    {
        solveOptimisedAuxLCANoneOfA1A2A3(nodes, P, result);
        solveOptimisedAuxLCANoneOfA1A2A3(nodes, reversedP, result);
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

        const int T = 1;
        cout << T << endl;

        for (int t = 0; t < T; t++)
        {
            const int maxN = 1000;
            const int N = rand() % maxN + 1;

#if 1
            vector<int> P = {1, 2, 3};
            random_shuffle(P.begin(), P.end());
#else
            // TODO - remove this!
            const int blah = rand() % 4;
            vector<int> P;
            switch (blah)
            {
                case 0:
                    P = {2, 1, 3};
                    break;
                case 1:
                    P = {3, 1, 2};
                    break;
                case 2:
                    P = {1, 3, 2};
                    break;
                case 3:
                    P = {2, 3, 1};
                    break;
            }
#endif

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

            //cout << " nodes: " << (u + 1) << " and " << (v + 1) << " are neighbours" << endl;

            nodes[u].children.push_back(&(nodes[v]));
            nodes[v].children.push_back(&(nodes[u]));

        }
#ifdef BRUTE_FORCE
        const auto solutionBruteForce = solveBruteForce(nodes, P).size();
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
        const auto solutionOptimised = solveOptimised2(nodes, P);
        cout << "solutionOptimised: " << solutionOptimised << endl;

        assert(solutionOptimised == solutionBruteForce);
#else
        const auto solutionOptimised2 = solveOptimised2(nodes, P);
        cout << solutionOptimised2 << endl;

#endif
    }


}




