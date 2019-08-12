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

void addTriplesWhereLCAIsNotA2(const vector<Node>& nodes, const Triple& P, int64_t& result)
{
    const bool isA2LessThanA1 = (P[1] < P[0]);
    const bool isA2LessThanA3 = (P[1] < P[2]);

    const auto numNodes = nodes.size();

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

void addTriplesWhereLCAIsA2(Node* currentNode, SegmentTree& nodeTracker, const Triple& P, int64_t& result)
{
    const bool isA2LessThanA1 = (P[1] < P[0]);
    const bool isA2LessThanA3 = (P[1] < P[2]);

    const auto nodeId = currentNode->id;

    // What if we are a1 or a3?
    nodeTracker.addValueAt(1, nodeId);

    // What if we are a2?
    const auto initialNumGreaterThan = nodeTracker.numToRightOf(nodeId);
    const auto initialNumLessThan = nodeTracker.numToLeftOf(nodeId);;
    auto descendantsGreaterThanSoFar = 0;
    auto descendantsLessThanSoFar = 0;

    for (Node* childNode : currentNode->children)
    {

        addTriplesWhereLCAIsA2(childNode, nodeTracker, P, result);
        const auto numLess = nodeTracker.numToLeftOf(nodeId);
        const auto numGreater = nodeTracker.numToRightOf(nodeId);
        const auto numGreaterThanViaThisChild = (numGreater - initialNumGreaterThan) - descendantsGreaterThanSoFar;
        const auto numLessThanViaThisChild = (numLess - initialNumLessThan) - descendantsLessThanSoFar;

        if (isA2LessThanA1 && isA2LessThanA3)
        {
            result += static_cast<int64_t>(numGreaterThanViaThisChild) * descendantsGreaterThanSoFar;
        }
        else if (!isA2LessThanA1 && !isA2LessThanA3)
        {
            result += static_cast<int64_t>(numLessThanViaThisChild) * descendantsLessThanSoFar;
        }
        else 
        {
            result += static_cast<int64_t>(numLessThanViaThisChild) * descendantsGreaterThanSoFar;
            result += static_cast<int64_t>(numGreaterThanViaThisChild) * descendantsLessThanSoFar;
        }

        descendantsGreaterThanSoFar += numGreaterThanViaThisChild;
        descendantsLessThanSoFar += numLessThanViaThisChild;
    }

    currentNode->numDescendentsLessThan = descendantsLessThanSoFar;
    currentNode->numDescendentsGreaterThan = descendantsGreaterThanSoFar;
}

int64_t solveOptimised2(vector<Node>& nodes, const Triple& P)
{
    int64_t result = 0;
    auto rootNode = &(nodes.front());

    fixParentChild(rootNode, nullptr);

    SegmentTree nodeTracker(nodes.size() + 1);
    addTriplesWhereLCAIsA2(rootNode, nodeTracker, P, result);

    addTriplesWhereLCAIsNotA2(nodes, P, result);
    const bool isPMonotonic = (P[2] > P[1] && P[1] > P[0]) || (P[2] < P[1] && P[1] < P[0]);
    if (isPMonotonic)
    {
        const Triple& reversedP = { { P[2], P[1], P[0] } };
        addTriplesWhereLCAIsNotA2(nodes, reversedP, result);
    }
    return result;
}

int main(int argc, char* argv[])
{
    // Ouch - tough one that took me an embarrassingly long time to
    // figure out XD.  Quite straightforward once you know how, though :)
    //
    // Let's start with a few definitions.
    //
    // Say that a triple (a1, a2, a3) is a "path-triple" if a2 lies on the
    // shortest path between a1 and a3.
    //
    // Given P = (p1, p2, p3), say that (a1, a2, a3) ~ P if and only if
    // for each 1 <= i, j <= 3, pi < pj => ai < aj.
    //
    // Say that P = (p1, p2, p3) is monotic if P is one of {(1, 2, 3), (3, 2, 1)};
    // else it is non-monotonic.
    //
    // Further split the non-monotonic as follows: say that P is a non-monotonic valley
    // if P is one of {(1, 3, 2), (2, 3, 1)}.  Say that P is a non-monotonic hill if P
    // is one of {(2, 1, 3), (3, 1, 2)}.
    //
    // We say that a path-triple (a1, a2, a3) is monotonic if (a1, a2, a3) ~ some monotonic
    // P, with similar definitions for (a1, a2, a3) being non-monotonic (and a hill, a valley, etc).
    //
    // The first problem I ran into was as follows: given a simple example of a Tree - 
    // a simple "line" of vertices, where the "endpoints" have degree 1 and all other
    // vertices have degree 2 - how can we find the number of non-monotonic path-triples
    // on this tree?
    //
    // We can easily see how we might calculate the number of *monotonic* path-triples: for each
    // vertex v, just set a2 = v and compute the number of a1's to the "left" of a2 on the line
    // that are less than a2 and the number of a3's to the right of a2 on the line with a3 > a2
    // (for P = (1, 2, 3) - P = (3, 2, 1) is similar).  But for monotonic P, the situation looks
    // more complex: if P = (2, 1, 3), say, then counting the number of a1's to the left of a2
    // with a1 > a2 and the a3's to the right of a2 with a3 > a2 appears to be of no help,
    // as we require further that a1 < a3.
    //
    // This had me stumped for an embarrassingly long time before I finally realised: this doesn't matter
    // at all! If we've found a path-triple (a1, a2, a3) with a1 > a2 and a3 > a2, then precisely one 
    // of the following holds:
    //
    //   i) a1 < a3, in which case (a1, a2, a3) ~ P; *or*
    //   ii) a3 > a1, in which case (a3, a2, a1) is a path-triple and (a3, a2, a1) ~ P.
    //
    // That is, the requirement that a1 < a3 poses no extra difficulty: precisely one of (a1, a2, a3) or
    // (a3, a2, a1) will ~ P!
    //
    // So for non-monotonic P, we have the following:
    //
    // Lemma 1
    //
    // Let P be monotonic.  Let S be a set of triples with the following property: for any triple (a1, a2, a3),
    // precisely one of (a1, a2, a3) and (a3, a2, a1) is in S.  Then is P is a non-monotonic hill, the number
    // of path-triples (a1, a2, a3) in T such that (a1, a2, a3) ~ P is simply the number of triples (a1, a2, a3)
    // in S such that (a1, a2, a3) is a path-triple and a1 > a2 and a3 > a2 (if P is a non-monotonic valley) or
    // the number of triples (a1, a2, a3) in S such that (a1, a2, a3) is a path-triple and a1 < a2 and a3 < a2 
    // (if P is a non-monotonic hill)
    //
    // Proof 
    // 
    // Follows from the above.
    //
    // QED
    //
    // The partitioning of triples into S assures that we don't "overcount" for non-monotonic P by counting the contributions
    // of both (a1, a2, a3) and its "reverse" path-triple (a3, a2, a1); we'll see, this
    // partitioning falls out quite naturally with little effort required, so poses no additional burden.
    //
    // Ok, we're ready to start describing how to actually solve the problem, now!
    //
    // Pick any node as a "root" node - I always pick node "1", but any will do - and perform a DFS.  It now makes sense
    // to say that a node u is a "descendent" of a node v or a node u' is an ancestor of node v' using the
    // well-known definitions of these terms. Here's another lemma:
    //
    // Lemma 2
    //
    // Given any path-triple (a1, a2, a3), at least one of a1 and a3 is a descendent of a2.
    //
    // Proof
    //
    // Left as an exercise for the reader ;)

    ios::sync_with_stdio(false);


    const auto T = read<int>();
    for (auto t = 0; t < T; t++)
    {
        const auto N = read<int>();

        vector<Node> nodes(N);
        for (auto i = 0; i < N; i++)
        {
            nodes[i].id = i + 1;
        }

        Triple P;
        for (auto& p : P)
        {
            p = read<int>();
        }

        for (auto i = 0; i < N - 1; i++)
        {
            const auto u = read<int>() - 1;
            const auto v = read<int>() - 1;

            nodes[u].children.push_back(&(nodes[v]));
            nodes[v].children.push_back(&(nodes[u]));

        }
        const auto solutionOptimised2 = solveOptimised2(nodes, P);
        cout << solutionOptimised2 << endl;
    }
}




