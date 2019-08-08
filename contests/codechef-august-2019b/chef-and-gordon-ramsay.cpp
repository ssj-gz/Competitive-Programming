// Simon St James (ssjgz) - 2019-08-08
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <array>
#include <algorithm>

#include <cassert>

using namespace std;

// Lots of input to read, so use ultra-fast reader.
void scan_integer( int &x )
{
    int c = getchar_unlocked();
    x = 0;
    for( ; ((c<48 || c>57) && c != '-'); c = getchar_unlocked() );
    for( ;c>47 && c<58; c = getchar_unlocked() ) {
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
};

using Triple = std::array<int, 3>;

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
        int numToRightOf(int pos)
        {
            return numInRange(pos + 1, m_size);
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

        // Find the number in the given range (inclusive) in O(log2(numElements)).
        int numInRange(int start, int end) const
        {
            start++; // Make 1-relative.  start and end are inclusive.
            end++;
            int sum = 0;
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
};

void solveOptimisedAuxLCANoneOfA1A2A3(const vector<Node>& nodes, const Triple& P, int64_t& result)
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

void solveOptimisedAuxLCAIsA2(Node* currentNode, SegmentTree& nodeTracker, const Triple& P, int64_t& result)
{
    const bool isA2LessThanA1 = (P[1] < P[0]);
    const bool isA2LessThanA3 = (P[1] < P[2]);

    const int nodeId = currentNode->id;

    // What if we are a1 or a3?
    nodeTracker.addValueAt(1, nodeId);

    // What if we are a2?
    const auto initialNumGreaterThan = nodeTracker.numToRightOf(nodeId);
    const auto initialNumLessThan = nodeTracker.numToLeftOf(nodeId);;
    int descendantsGreaterThanSoFar = 0;
    int descendantsLessThanSoFar = 0;

    for (Node* childNode : currentNode->children)
    {

        solveOptimisedAuxLCAIsA2(childNode, nodeTracker, P, result);
        const int numLess = nodeTracker.numToLeftOf(nodeId);
        const int numGreater = nodeTracker.numToRightOf(nodeId);
        const int numOfThisChildGreaterThan = (numGreater - initialNumGreaterThan) - descendantsGreaterThanSoFar;
        const int numOfThisChildLessThan = (numLess - initialNumLessThan) - descendantsLessThanSoFar;

        if (isA2LessThanA1 && isA2LessThanA3)
        {
            result += static_cast<int64_t>(numOfThisChildGreaterThan) * descendantsGreaterThanSoFar;
        }
        else if (!isA2LessThanA1 && !isA2LessThanA3)
        {
            result += static_cast<int64_t>(numOfThisChildLessThan) * descendantsLessThanSoFar;
        }
        else 
        {
            result += static_cast<int64_t>(numOfThisChildLessThan) * descendantsGreaterThanSoFar;
            result += static_cast<int64_t>(numOfThisChildGreaterThan) * descendantsLessThanSoFar;
        }

        descendantsGreaterThanSoFar += numOfThisChildGreaterThan;
        descendantsLessThanSoFar += numOfThisChildLessThan;
    }

    currentNode->numDescendentsLessThan = descendantsLessThanSoFar;
    currentNode->numDescendentsGreaterThan = descendantsGreaterThanSoFar;
}

int64_t solveOptimised2(vector<Node>& nodes, const Triple& P)
{
    int64_t result = 0;
    auto rootNode = &(nodes.front());
    const Triple& reversedP = { P[2], P[1], P[0] };

    fixParentChild(rootNode, nullptr);

    SegmentTree nodeTracker(nodes.size() + 1);
    solveOptimisedAuxLCAIsA2(rootNode, nodeTracker, P, result);

    const bool isPMonotonic = (P[2] > P[1] && P[1] > P[0]) || (P[2] < P[1] && P[1] < P[0]);
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

    const int T = read<int>();
    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();

        vector<Node> nodes(N);
        for (int i = 0; i < N; i++)
        {
            nodes[i].id = i + 1;
        }

        Triple P;
        for (auto& p : P)
        {
            p = read<int>();
        }

        for (int i = 0; i < N - 1; i++)
        {
            const int u = read<int>() - 1;
            const int v = read<int>() - 1;

            nodes[u].children.push_back(&(nodes[v]));
            nodes[v].children.push_back(&(nodes[u]));

        }
        const auto solutionOptimised2 = solveOptimised2(nodes, P);
        cout << solutionOptimised2 << endl;
    }
}




