// Simon St James (ssjgz) - 2019-08-08
//#define SUBMISSION
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
        int numIdsGreaterThan(int nodeId)
        {
            return numInRange(nodeId + 1, m_size);
        }
        int64_t numIdsLessThan(int nodeId)
        {
            return numInRange(0, nodeId - 1);
        }

        // Add "value" to the current value at pos in O(log2(numElements)).
        void registerNodeId(int nodeId)
        {
            const auto n = m_numElements;
            auto elements = m_elements.data();
            int pos = nodeId + 1; // Make 1-relative.
            while(pos <= n)
            {
                elements[pos]++;
                assert(elements[pos] >= 0);
                pos += (pos & (pos * -1));
            }
        }
    private:
        int m_size;
        int m_numElements;
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

void addTriplesWhereLCAIsNotA2(const vector<Node>& nodes, const Triple& P, int64_t& numTriples)
{
    // Assumes that a3 is a descendent of a2, but a1 is not a descendent of a2.
    const bool isA2LessThanA1 = (P[1] < P[0]);
    const bool isA2LessThanA3 = (P[1] < P[2]);

    const auto numNodes = nodes.size();

    for (const Node& a2 : nodes)
    {

        const auto totalNumNodesGreaterThan = (numNodes - a2.id);
        const auto numNonDescendentNodesGreaterThan = totalNumNodesGreaterThan - a2.numDescendentsGreaterThan;

        const auto totalNumNodesLessThan = a2.id - 1;
        const auto numNonDescendentNodesLessThan = totalNumNodesLessThan - a2.numDescendentsLessThan;

        const auto numA1 = isA2LessThanA1 ? numNonDescendentNodesGreaterThan : numNonDescendentNodesLessThan;

        if (isA2LessThanA3)
        {
            numTriples += static_cast<int64_t>(numA1) * a2.numDescendentsGreaterThan;
        }
        else
        {
            numTriples += static_cast<int64_t>(numA1) * a2.numDescendentsLessThan;
        }

    }
}

void addTriplesWhereLCAIsA2(Node* currentNode, SegmentTree& nodeIdTracker, const Triple& P, int64_t& numTriples)
{
    const bool isA2LessThanA1 = (P[1] < P[0]);
    const bool isA2LessThanA3 = (P[1] < P[2]);

    const auto nodeId = currentNode->id;

    // What if we are a1 or a3? Register ourselves so that our ancestor a2's can count us among the
    // number of nodes less/ greater than a2.
    nodeIdTracker.registerNodeId(nodeId);

    // What if we are a2?
    const auto initialNumGreaterThan = nodeIdTracker.numIdsGreaterThan(nodeId);
    const auto initialNumLessThan = nodeIdTracker.numIdsLessThan(nodeId);
    auto descendantsGreaterThanSoFar = 0;
    auto descendantsLessThanSoFar = 0;

    for (Node* childNode : currentNode->children)
    {
        // Recurse, so that our descendents will register themselves in the nodeIdTracker.
        addTriplesWhereLCAIsA2(childNode, nodeIdTracker, P, numTriples);

        const auto numLess = nodeIdTracker.numIdsLessThan(nodeId);
        const auto numGreater = nodeIdTracker.numIdsGreaterThan(nodeId);
        const auto numGreaterThanViaThisChild = (numGreater - initialNumGreaterThan) - descendantsGreaterThanSoFar;
        const auto numLessThanViaThisChild = (numLess - initialNumLessThan) - descendantsLessThanSoFar;

        if (isA2LessThanA1 && isA2LessThanA3)
        {
            // Non-monotonic valley i.e. (2, 1, 3) or (3, 1, 2)
            numTriples += static_cast<int64_t>(numGreaterThanViaThisChild) * descendantsGreaterThanSoFar;
        }
        else if (!isA2LessThanA1 && !isA2LessThanA3)
        {
            // Non-monotonic hill i.e. (2, 3, 1) or (1, 3, 2)
            numTriples += static_cast<int64_t>(numLessThanViaThisChild) * descendantsLessThanSoFar;
        }
        else if (isA2LessThanA1 && !isA2LessThanA3)
        {
            // P = (3, 2, 1)
            numTriples += static_cast<int64_t>(descendantsGreaterThanSoFar) * numLessThanViaThisChild;
        }
        else if (!isA2LessThanA1 && isA2LessThanA3)
        {
            // P = (1, 2, 3)
            numTriples += static_cast<int64_t>(descendantsLessThanSoFar) * numGreaterThanViaThisChild;
        }
        else
        {
            assert(false);
        }

        descendantsGreaterThanSoFar += numGreaterThanViaThisChild;
        descendantsLessThanSoFar += numLessThanViaThisChild;
    }

    currentNode->numDescendentsLessThan = descendantsLessThanSoFar;
    currentNode->numDescendentsGreaterThan = descendantsGreaterThanSoFar;
}

int64_t calcNumTriples(vector<Node>& nodes, const Triple& P)
{
    int64_t numTriples = 0;
    auto rootNode = &(nodes.front());

    fixParentChild(rootNode, nullptr);

    // Count triples where both a3 and a1 are descendents of a2.
    // We make two passes, reversing the order of children between
    // pass 1 and pass 2.
    SegmentTree nodeIdTracker(nodes.size() + 1);
    addTriplesWhereLCAIsA2(rootNode, nodeIdTracker, P, numTriples);
    for (auto& node : nodes)
    {
        reverse(node.children.begin(), node.children.end());
    }
    addTriplesWhereLCAIsA2(rootNode, nodeIdTracker, P, numTriples);

    // Count triples where a3 is a descendent of a2 but a1 is not.
    // Two passes are made here: the second reverses the order of P.
    addTriplesWhereLCAIsNotA2(nodes, P, numTriples);
    const Triple& reversedP = { { P[2], P[1], P[0] } };
    addTriplesWhereLCAIsNotA2(nodes, reversedP, numTriples);

    const bool isPMonotonic = (P[2] > P[1] && P[1] > P[0]) || (P[2] < P[1] && P[1] < P[0]);
    if (!isPMonotonic)
    {
        // Recall from e.g. Lemma 1 that we will have *over* counted non-monotonic triples
        // by a factor of exactly 2: adjust for this.
        numTriples /= 2;
    }
    return numTriples;
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
    // Further split the non-monotonic as follows: say that P is a non-monotonic hill
    // if P is one of {(1, 3, 2), (2, 3, 1)}.  Say that P is a non-monotonic valley if P
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
    // that are less than a2 and the number of a3's to the "right" of a2 on the line with a3 > a2
    // (for P = (1, 2, 3) - the case for P = (3, 2, 1) is similar).  But for monotonic P, the situation looks
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
    // (a3, a2, a1) will ~ P! Note though that if we count both path-triples (a1, a2, a3) *and* (a3, a2, a1),
    // then we will count the same path-triple *twice*, so we'll need to divide by 2 at the end.
    //
    // So for non-monotonic P, we have the following:
    //
    // Lemma 1
    //
    // Let P be non-monotonic.  
    // Then if P is a non-monotonic valley, the number of of path-triples (a1, a2, a3) in T such that (a1, a2, a3) ~ P 
    // is simply the number of triples path-triples (a1, a2, a3) a1 > a2 and a3 > a2 *divided by 2*; similarly, if P 
    // is a non-monotonic hill, the number of path-triples (a1, a2, a3) (a1, a2, a3) ~ P is simply the number of 
    // path-triples with a1 < a2 and a3 < a2, again divided by 2.
    //
    // Proof 
    // 
    // Follows from the above.
    //
    // QED
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
    //
    // Lets insist for now that a3 is always a descendent of a2.  Then we have two cases:
    //
    //    i) a1 is also a descendent of a2.
    //    ii) a1 is not also a descendent of a2.
    //
    // How can we count all the path-triples (a1, a2, a3) ~ P satisfying case i) i.e. that
    // a1 and a3 are both descendents of a2? Well, in this case, since a2 is on the path
    // from a1 and a3, a2 must be the Least Common Ancestor (LCA) of a1 and a3.
    // This means, additionally, that the path from a2 to a1 and the path from a2 to a3
    // go through different *children* of a2.  Let c_1, c_2, ... c_m be the children of 
    // a2.  Let P = (1, 2, 3), for the purposes of exposition.
    //
    // How can we find all path-triples (a1, a2, a3) ~ P where a2 is the LCA of both a1 and
    // a3? It's hopefully clear that it is:
    //
    //   sum [ i = 1, ... m, j = 1, ... m, i != j ] { the number of descendents a1 of a2 reached
    //                                                via child c_i with a1 < a2 times by
    //                                                the number of descendents a3 of a2 reached
    //                                                via child c_j with a3 > a2 }
    //
    // This would be quadratic in the number of children of a2, but there are well-known more-efficient
    // ways of doing this - hopefully the code addTriplesWhereLCAIsA2(...) is clear.  Note that
    // addTriplesWhereLCAIsA2(...) is run *twice*, with the second pass iterating over the children
    // in the reverse order - this is necessary for finding all triples in the case where P is monotonic.  
    // For non-monotonic P, this will count the same path-triples twice, so we'll need to remember to divide by 2!
    //
    // addTriplesWhereLCAIsA2(...) computes, for each a2, the number of descendent nodes with ids less than
    // that of a2 and the number of descendent nodes with ids greater than that of a2 using a segment
    // tree (nodeIdTracker) - it's hopefully clear how this works.  The results are stored for later
    // use by addTriplesWhereLCAIsNotA2(...).
    //
    // How about case ii) i.e. how can we count all path-triples with a3 a descendent of a2 but
    // a1 *not* a descendent of a2? Let P = (1, 2, 3), again for the purposes of exposition.  It's hopefully obvious 
    // that for a given a2, the number of such path-triples ~ P is the number of nodes a1 which have id less than a2 and 
    // which are *not* descendents of a2 times by the number of nodes a3 which have id greater than 
    // a2 and which *are* descendents of a2.  How can we compute the number of nodes with id less/ greater than
    // that of a2 which are *not* descendents of a2? This is easy: addTriplesWhereLCAIsA2 already found
    // the number of lesser/ greater nodes that are descendents of a2, and it's easy to compute the total number
    // of nodes in the graph whose id is lesser/ greater than that of a2, so the calculation is simple:
    // see numNonDescendentNodesGreaterThan/ numNonDescendentNodesLessThan. addTriplesWhereLCAIsNotA2 handles all this.
    // Again, it needs to be run twice due to our insistence that a3 is a descendent of a1, this time with 
    // P reversed between the passes.  Once again, this will overcount non-monotonic triples by a factor of exactly 2,
    // so we'll need to remember to divide by 2 at the end :)
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
        const auto numTriples = calcNumTriples(nodes, P);
        cout << numTriples << endl;
    }
}




