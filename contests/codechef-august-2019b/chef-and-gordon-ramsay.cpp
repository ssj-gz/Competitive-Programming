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

using namespace std;

#define gc getchar_unlocked

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
                cout << "Found! (" << startNode->id << ", " << nodeinPath->id << ", " << currentNode->id << ")" << endl;
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

void solveOptimisedAuxLCANoneOfA1A2A3(Node* currentNode, Node* parentNode, SegmentTree& a1Tracker, SegmentTree& a2WithA1Tracker, const Triple& P, int64_t& result)
{
    const bool isA2LessThanA1 = (P[1] < P[0]);
    const bool isA2LessThanA3 = (P[1] < P[2]);

    //cout << "visiting node: " << currentNode->id << endl;
    // What if we are a3?
    const auto numA2WithA1 =  isA2LessThanA3 ? a2WithA1Tracker.numToLeftOf(currentNode->id) : a2WithA1Tracker.numToRightOf(currentNode->id);
    //cout << " numA2WithA1 for a3 " << currentNode->id << " = " << numA2WithA1 << endl;
    //cout << "** added " << numA2WithA1 << " to result for a3 = " << currentNode->id << endl;
    result += numA2WithA1;

    // What if we are a2?
    const auto numA1 = isA2LessThanA1 ? a1Tracker.numToRightOf(currentNode->id) : a1Tracker.numToLeftOf(currentNode->id);
    //cout << " numA1 for a2 " << currentNode->id << " = " << numA1 << endl;
    //cout << " added " << numA1 << " to a2WithA1Tracker at pos: " << currentNode->id << endl;
    if (numA1 != 0)
        a2WithA1Tracker.addValueAt(numA1, currentNode->id);

    for (Node* childNode : currentNode->neighbours)
    {
        if (childNode == parentNode)
            continue;

        solveOptimisedAuxLCANoneOfA1A2A3(childNode, currentNode, a1Tracker, a2WithA1Tracker, P, result);
    }

    // What if we are a1?
    a1Tracker.addValueAt(1, currentNode->id);
    //cout << " added 1 to a1Tracker at pos: " << currentNode->id << endl;

    // What if we are a2, and have finished this node? We are not an a2 any more.
    if (numA1 != 0)
        a2WithA1Tracker.addValueAt(-numA1, currentNode->id);



    //cout << "finished node: " << currentNode->id << endl;
}

void solveOptimisedAuxLCAIsA2(Node* currentNode, Node* parentNode, SegmentTree& nodeTracker, const Triple& P, int64_t& result)
{
    const bool isA2LessThanA1 = (P[1] < P[0]);
    const bool isA2LessThanA3 = (P[1] < P[2]);

    const bool isMonotic = (isA2LessThanA1 != isA2LessThanA3);

    // What if we are a2?
    const auto initialNumGreaterThan = nodeTracker.numToRightOf(currentNode->id);
    const auto initialNumLessThan = nodeTracker.total() - initialNumGreaterThan;
    int64_t descendantsGreaterThanSoFar = 0;
    int64_t descendantsLessThanSoFar = 0;

    //cout << " at node: " << currentNode->id << " initialNumGreaterThan: " << initialNumGreaterThan << endl;

    for (Node* childNode : currentNode->neighbours)
    {
        if (childNode == parentNode)
            continue;

        solveOptimisedAuxLCAIsA2(childNode, currentNode, nodeTracker, P, result);
        const int numGreater = nodeTracker.numToRightOf(currentNode->id);
        const int numLess = nodeTracker.total() - numGreater;
        const auto numOfThisChildGreaterThan = (numGreater - initialNumGreaterThan) - descendantsGreaterThanSoFar;
        const auto numOfThisChildLessThan = (numLess - initialNumLessThan) - descendantsLessThanSoFar;
        //cout << "  at node: " << currentNode << " explored child: " << childNode->id << " numOfThisChildGreaterThan: " << numOfThisChildGreaterThan << " descendantsGreaterThanSoFar: " << descendantsGreaterThanSoFar << "  numOfThisChildLessThan: " << numOfThisChildLessThan << " descendantsLessThanSoFar: "<< descendantsLessThanSoFar << endl;

        //cout << " isA2LessThanA1: " << isA2LessThanA1 << " isA2LessThanA3: " << isA2LessThanA3 << endl;

        if (isA2LessThanA1 && isA2LessThanA3)
        {
            //cout << " glarp" << endl;
            result += numOfThisChildGreaterThan * descendantsGreaterThanSoFar;
        }
        else if (!isA2LessThanA1 && !isA2LessThanA3)
        {
            //cout << " glorp" << endl;
            result += numOfThisChildLessThan * descendantsLessThanSoFar;
        }
        else 
        {
            //cout << " lca is a2 node: " << currentNode->id << " adding " << (numOfThisChildLessThan * descendantsGreaterThanSoFar) << " to result" << endl;
            result += numOfThisChildLessThan * descendantsGreaterThanSoFar;
            result += numOfThisChildGreaterThan * descendantsLessThanSoFar;
        }

        descendantsGreaterThanSoFar += numOfThisChildGreaterThan;
        descendantsLessThanSoFar += numOfThisChildLessThan;
    }

    // What if we are a1 or a3?
    nodeTracker.addValueAt(1, currentNode->id);
}

void solveOptimisedAuxLCAIsA1(Node* currentNode, Node* parentNode, SegmentTree& a1Tracker, SegmentTree& a2WithA1Tracker, const Triple& P, int64_t& result)
{
    const bool isA2LessThanA1 = (P[1] < P[0]);
    const bool isA2LessThanA3 = (P[1] < P[2]);

    // What if we are a3?
    const auto numA2WithA1 = isA2LessThanA3 ? a2WithA1Tracker.numToLeftOf(currentNode->id) : a2WithA1Tracker.numToRightOf(currentNode->id);

    // What if we are a2? 
    const auto numA1 = isA2LessThanA1 ? a1Tracker.numToRightOf(currentNode->id) : a1Tracker.numToLeftOf(currentNode->id);
    if (numA1 != 0)
        a2WithA1Tracker.addValueAt(numA1, currentNode->id);

    // What if we are a1?
    a1Tracker.addValueAt(1, currentNode->id);


    //cout << " numA2WithA1 for a3 " << currentNode->id << " = " << numA2WithA1 << endl;
    //cout << "** added " << numA2WithA1 << " to result for a3 = " << currentNode->id << endl;
    result += numA2WithA1;

    for (Node* childNode : currentNode->neighbours)
    {
        if (childNode == parentNode)
            continue;
        solveOptimisedAuxLCAIsA1(childNode, currentNode, a1Tracker, a2WithA1Tracker, P, result);
    }

    // What if we are a2, and have finished this node? We are not an a2 any more.
    if (numA1 != 0)
        a2WithA1Tracker.addValueAt(-numA1, currentNode->id);

    // What if we are a1, and have finished this node? We are not an a1 any more.
    a1Tracker.addValueAt(-1, currentNode->id);
}

int64_t solveOptimised2(vector<Node>& nodes, const array<int, 3>& Parray)
{
    int64_t result = 0;
    auto rootNode = &(nodes.front());
    const Triple P = { Parray[0], Parray[1], Parray[2] };
    const Triple reversedP = { Parray[2], Parray[1], Parray[0] };

    const bool isPMonotonic = (P[2] > P[1] && P[1] > P[0]) || (P[2] < P[1] && P[1] < P[0]);

    if (!isPMonotonic)
    {
        {
            //cout << "Forward pass" << endl;
            SegmentTree a1Tracker(nodes.size() + 1);
            SegmentTree a2WithA1Tracker(nodes.size() + 1);
            solveOptimisedAuxLCANoneOfA1A2A3(rootNode, nullptr, a1Tracker, a2WithA1Tracker, P, result);
        }
        {
            for (auto& node : nodes)
            {
                reverse(node.neighbours.begin(), node.neighbours.end());
            }
            //cout << "Backward pass" << endl;
            SegmentTree a1Tracker(nodes.size() + 1);
            SegmentTree a2WithA1Tracker(nodes.size() + 1);
            solveOptimisedAuxLCANoneOfA1A2A3(rootNode, nullptr, a1Tracker, a2WithA1Tracker, P, result);
        }
        {
            //cout << " a2 is LCA" << endl;
            SegmentTree nodeTracker(nodes.size() + 1);
            solveOptimisedAuxLCAIsA2(rootNode, nullptr, nodeTracker, P, result);
        }
        {
            //cout << " a1 is LCA" << endl;
            SegmentTree a1Tracker(nodes.size() + 1);
            SegmentTree a2WithA1Tracker(nodes.size() + 1);
            solveOptimisedAuxLCAIsA1(rootNode, nullptr, a1Tracker, a2WithA1Tracker, P, result);
        }
    }
    else
    {
        {
            //cout << "Forward pass P forward" << endl;
            SegmentTree a1Tracker(nodes.size() + 1);
            SegmentTree a2WithA1Tracker(nodes.size() + 1);
            solveOptimisedAuxLCANoneOfA1A2A3(rootNode, nullptr, a1Tracker, a2WithA1Tracker, P, result);
        }
        {
            //cout << "Forward pass P backward" << endl;
            SegmentTree a1Tracker(nodes.size() + 1);
            SegmentTree a2WithA1Tracker(nodes.size() + 1);
            solveOptimisedAuxLCANoneOfA1A2A3(rootNode, nullptr, a1Tracker, a2WithA1Tracker, reversedP, result);
        }
        {
            for (auto& node : nodes)
            {
                reverse(node.neighbours.begin(), node.neighbours.end());
            }
            //cout << "Backward pass P forward" << endl;
            SegmentTree a1Tracker(nodes.size() + 1);
            SegmentTree a2WithA1Tracker(nodes.size() + 1);
            solveOptimisedAuxLCANoneOfA1A2A3(rootNode, nullptr, a1Tracker, a2WithA1Tracker, P, result);
        }
        {
            //cout << "Backward pass P backward" << endl;
            SegmentTree a1Tracker(nodes.size() + 1);
            SegmentTree a2WithA1Tracker(nodes.size() + 1);
            solveOptimisedAuxLCANoneOfA1A2A3(rootNode, nullptr, a1Tracker, a2WithA1Tracker, reversedP, result);
        }
        {
            //cout << " a1 is LCA P forward" << endl;
            SegmentTree a1Tracker(nodes.size() + 1);
            SegmentTree a2WithA1Tracker(nodes.size() + 1);
            solveOptimisedAuxLCAIsA1(rootNode, nullptr, a1Tracker, a2WithA1Tracker, P, result);
        }
        {
            //cout << " a1 is LCA P backward" << endl;
            SegmentTree a1Tracker(nodes.size() + 1);
            SegmentTree a2WithA1Tracker(nodes.size() + 1);
            solveOptimisedAuxLCAIsA1(rootNode, nullptr, a1Tracker, a2WithA1Tracker, reversedP, result);
        }
        {
            //cout << " a2 is LCA forward" << endl;
            SegmentTree nodeTracker(nodes.size() + 1);
            solveOptimisedAuxLCAIsA2(rootNode, nullptr, nodeTracker, P, result);
        }
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

        const int T = 20;
        cout << T << endl;

        for (int t = 0; t < T; t++)
        {
            const int maxN = 100'000;
            //const int N = rand() % maxN + 1;
            const int N = 100'000;

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

            nodes[u].neighbours.push_back(&(nodes[v]));
            nodes[v].neighbours.push_back(&(nodes[u]));

        }
#ifdef BRUTE_FORCE
        const auto solutionBruteForce = solveBruteForce(nodes, P).size();
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
        const auto solutionOptimised = solveOptimised2(nodes, P);
        cout << "solutionOptimised: " << solutionOptimised << endl;

        assert(solutionOptimised == solutionBruteForce);
#else
        const auto solutionOptimised = solveOptimised2(nodes, P);
        cout << solutionOptimised << endl;
#endif
    }


}


