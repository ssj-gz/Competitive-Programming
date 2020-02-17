// Simon St James (ssjgz) - Setter's solution for "Equilateral Treeangles" - 2019-07-17.
// Problem was later renamed to "Chef and Gordon Ramsay 2" when I saw CHGORAM in AUG19
// and noticed how my Problem could be turned into a sequel to it :)
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

#include <cassert>

using namespace std;

const int numTripletPermutations = 1 * 2 * 3; // i.e. == factorial(3).

struct DescendantWithHeightInfo
{
    // The number of suitable pairs of descendants of Node with the given height
    // that have Node as their Lowest Common Ancestor.
    int64_t numPairsWithHeightWithNodeAsLCA = 0;
    // The name "number" is not strictly correct: if numPairsWithHeightWithNodeAsLCA > 0, then this
    // will indeed be the number of suitable descendants of this Node which have this height,
    // but if numPairsWithHeightWithNodeAsLCA == 0, "number" will also be 0.
    // 
    // The "number" count is used to avoid the overcount when performing completeTripletsOfTypeA
    // (Centroid Decomposition has no notion of our parent-child relationship, so
    // will count "descendants" of a Node alongside "non-descendants" of a Node.)
    int number = 0;
};

struct Node
{
    vector<Node*> neighbours;
    bool isSuitable = false;
    int height = 0;

    // The total number of entries into descendantWithHeightInfo, summed across *all* nodes,
    // will be O(n log n).
    map<int, DescendantWithHeightInfo> descendantWithHeightInfo;
};

struct HeightInfo
{
    int numWithHeight = 0;
    // Make a note of which Node this info has been incorporated into (i.e. which Node's
    // descendantWithHeightInfo it has been used to update) so we don't accidentally
    // incorporate it into the same Node twice!
    Node* lastIncorporatedIntoNode = nullptr;
};

class DistTracker
{
    public:
        // O(maxDist).
        DistTracker(int maxDist)
            : m_numWithDist(2 * maxDist + 1), 
            m_maxDist(maxDist)
        {
        }
        // O(1).
        void insertDist(const int newDist)
        {
            numWithDistValue(newDist)++;
            m_largestDist = max(m_largestDist, newDist);
        };
        // O(1).
        int numWithDist(int dist)
        {
            return numWithDistValue(dist);
        }
        // O(1).
        void adjustAllDists(int distDiff)
        {
            m_cumulativeDistAdjustment += distDiff;
            assert(m_cumulativeDistAdjustment >= 0);
            if (m_largestDist != -1)
                m_largestDist += distDiff;
        }
        // O(1).
        int largestDist() const
        {
            return m_largestDist;
        }
    private:
        int m_cumulativeDistAdjustment = 0;
        vector<int> m_numWithDist;
        int m_maxDist = -1;

        int m_largestDist = -1;

        int& numWithDistValue(int dist)
        {
            return m_numWithDist[dist - m_cumulativeDistAdjustment + m_maxDist];
        }
};

enum DistTrackerAdjustment {DoNotAdjust, AdjustWithDepth};

template <typename NodeProcessor>
void doDfs(Node* node, Node* parentNode, int depth, DistTracker& distTracker, DistTrackerAdjustment distTrackerAdjustment, NodeProcessor& processNode)
{
    if (distTrackerAdjustment == AdjustWithDepth)
        distTracker.adjustAllDists(1);

    processNode(node, depth, distTracker);

    for (auto child : node->neighbours)
    {
        if (child == parentNode)
            continue;
        doDfs(child, node, depth + 1, distTracker, distTrackerAdjustment, processNode);
    }

    if (distTrackerAdjustment == AdjustWithDepth)
        distTracker.adjustAllDists(-1);
}

int countDescendants(Node* node, Node* parentNode)
{
    auto numDescendants = 1; // Current node.

    for (const auto& child : node->neighbours)
    {
        if (child == parentNode)
            continue;

        numDescendants += countDescendants(child, node);
    }

    return numDescendants;
}

int findCentroidAux(Node* currentNode, Node* parentNode, const int totalNodes, Node** centroid)
{
    auto numDescendants = 1;

    auto childHasTooManyDescendants = false;

    for (const auto& child : currentNode->neighbours)
    {
        if (child == parentNode)
            continue;

        const auto numChildDescendants = findCentroidAux(child, currentNode, totalNodes, centroid);
        if (numChildDescendants > totalNodes / 2)
        {
            // Not the centroid, but can't break here - must continue processing children.
            childHasTooManyDescendants = true;
        }

        numDescendants += numChildDescendants;
    }

    if (!childHasTooManyDescendants)
    {
        // No child has more than totalNodes/2 descendants, but what about the remainder of the graph?
        const auto nonChildDescendants = totalNodes - numDescendants;
        if (nonChildDescendants <= totalNodes / 2)
        {
            assert(centroid);
            *centroid = currentNode;
        }
    }

    return numDescendants;
}

Node* findCentroid(Node* startNode)
{
    const auto totalNumNodes = countDescendants(startNode, nullptr);
    Node* centroid = nullptr;
    findCentroidAux(startNode, nullptr, totalNumNodes, &centroid);
    assert(centroid);
    return centroid;
}

void doCentroidDecomposition(Node* startNode, int64_t& numTriplets)
{
    Node* centroid = findCentroid(startNode);

    auto addSomeTypeATripletsForNode = [&numTriplets](Node* node, DistTracker& distTracker)
    {
        // This will be called O(log2 n) times for each node before that node's
        // Type A Triplets are fully completed.
        for (const auto& heightPair : node->descendantWithHeightInfo)
        {
            const auto descendantHeight = heightPair.first;
            const auto requiredNonDescendantDist = (descendantHeight - node->height);
            if (requiredNonDescendantDist > distTracker.largestDist())
                break; // Optimisation - no point continuing with larger descendantHeights.

            const auto numPairsWithHeightWithNodeAsLCA = heightPair.second.numPairsWithHeightWithNodeAsLCA;
            const auto numNewTriplets = numPairsWithHeightWithNodeAsLCA * distTracker.numWithDist(requiredNonDescendantDist) * numTripletPermutations;
            assert(numNewTriplets >= 0);
            numTriplets += numNewTriplets;
        }
    };
    auto propagateDists = [&addSomeTypeATripletsForNode](Node* node, int depth, DistTracker& distTracker)
                        {
                            addSomeTypeATripletsForNode(node, distTracker);
                        };
    auto collectDists = [](Node* node, int depth, DistTracker& distTracker)
                        {
                            if (node->isSuitable)
                                distTracker.insertDist(depth);
                        };

    const auto numNodesInComponent = countDescendants(startNode, nullptr);

    {
        DistTracker distTracker(numNodesInComponent);
        for (auto& child : centroid->neighbours)
        {
            doDfs(child, centroid, 1, distTracker, AdjustWithDepth, propagateDists );
            doDfs(child, centroid, 1, distTracker, DoNotAdjust, collectDists );
        }
    }
    {
        DistTracker distTracker(numNodesInComponent);
        // Do it again, this time backwards ...
        reverse(centroid->neighbours.begin(), centroid->neighbours.end());
        // ... and also include the centre, this time.
        if (centroid->isSuitable)
            distTracker.insertDist(0);
        for (auto& child : centroid->neighbours)
        {
            doDfs(child, centroid, 1, distTracker, AdjustWithDepth, propagateDists );
            doDfs(child, centroid, 1, distTracker, DoNotAdjust, collectDists );
        }
        addSomeTypeATripletsForNode(centroid, distTracker);
    }

    for (auto& neighbour : centroid->neighbours)
    {
        assert(std::find(neighbour->neighbours.begin(), neighbour->neighbours.end(), centroid) != neighbour->neighbours.end());
        // Erase the edge from the centroid's neighbour to the centroid, essentially "chopping off" each child into its own
        // component ...
        neighbour->neighbours.erase(std::find(neighbour->neighbours.begin(), neighbour->neighbours.end(), centroid));
        // ... and recurse.
        doCentroidDecomposition(neighbour, numTriplets);
    }
}

void completeTripletsOfTypeACentroidDecomposition(vector<Node>& nodes, Node* rootNode, int64_t& numTriplets)
{
    doCentroidDecomposition(rootNode, numTriplets);
    // Fix the overcount caused by Centroid Decomposition (over-)counting descendants of a node as non-descendants
    // of a node!
    for (auto& node : nodes)
    {
        for (const auto descendantHeightPair : node.descendantWithHeightInfo)
        {
            const auto height = descendantHeightPair.first;
            const auto numPairsWithHeightWithNodeAsLCA = descendantHeightPair.second.numPairsWithHeightWithNodeAsLCA;

            // Centroid decomposition would have (wrongly) added numPairsWithHeightWithNodeAsLCA[height] * numTripletPermutations 
            // for each suitable descendant of node with height "height" - correct for this.
            numTriplets -= numPairsWithHeightWithNodeAsLCA * node.descendantWithHeightInfo[height].number * numTripletPermutations;
        }
    }
}

map<int, HeightInfo> buildDescendantHeightInfo(Node* currentNode, Node* parentNode, int height, int64_t& numTriplets)
{
    currentNode->height = height;

    map<int, HeightInfo> persistentInfoForDescendantHeight;

    for (auto child : currentNode->neighbours)
    {
        if (child == parentNode)
            continue;
        // Quick C++ performance note: in C++11 onwards, capturing a returned std::map
        // in a local variable is O(1), due to Move Semantics.  Prior to this, though,
        // it could have been O(size of std::map) (if the Return Value Optimisation ended up
        // not being used), which would (silently!) lead to asymptotically worse performance!
        //
        // Luckily, this code uses C++11 features so we can't accidentally fall into this trap.
        auto transientInfoForDescendantHeight = buildDescendantHeightInfo(child, currentNode, height + 1, numTriplets);
        if (transientInfoForDescendantHeight.size() > persistentInfoForDescendantHeight.size())
        {
            // We'll be copying transientInfoForDescendantHeight into persistentInfoForDescendantHeight.
            // Ensure that the former is smaller than the latter so that we can make use of the Small-to-Large
            // trick. NB: std::swap'ing is O(1).
            swap(persistentInfoForDescendantHeight, transientInfoForDescendantHeight);
        }

        for (auto transientDescendantHeightPair : transientInfoForDescendantHeight)
        {
            // This block of code (i.e. the body of the containing for... loop) 
            // is executed O(n log2 n) times over the whole run.
            // It is guaranteed to be executed with descendantHeight if the current
            // child has a descendant with height descendantHeight that isSuitable and a previous child of this
            // node also has a descendant with height descendantHeight that isSuitable, but may also
            // be executed under different circumstances.
            //
            // Since this block of code adds at most one entry into currentNode's descendantWithHeightInfo member,
            // the sum of node->descendantWithHeightInfo.size() over all nodes is O(n log2 n).
            const auto descendantHeight = transientDescendantHeightPair.first;

            const auto& transientHeightInfo = transientDescendantHeightPair.second;
            auto& heightInfoForNode = persistentInfoForDescendantHeight[descendantHeight];

            assert (descendantHeight > currentNode->height);

            auto numUnprocessedDescendantsWithHeight = -1;
            auto numKnownDescendantsWithHeight = -1;
            assert(transientHeightInfo.lastIncorporatedIntoNode != nullptr);
            if (transientHeightInfo.lastIncorporatedIntoNode == currentNode)
            {
                assert(heightInfoForNode.lastIncorporatedIntoNode != currentNode);
                numUnprocessedDescendantsWithHeight = heightInfoForNode.numWithHeight;
                numKnownDescendantsWithHeight = transientHeightInfo.numWithHeight;
            }
            else
            {
                assert(transientHeightInfo.lastIncorporatedIntoNode != currentNode);
                numUnprocessedDescendantsWithHeight = transientHeightInfo.numWithHeight;
                numKnownDescendantsWithHeight = heightInfoForNode.numWithHeight;
            }

            const auto earlierChildHasThisHeight = (numKnownDescendantsWithHeight > 0);
            if (earlierChildHasThisHeight)
            {
                // Incorporate any un-incorporated HeightInfo into this Node's descendantWithHeightInfo.
                auto& descendantHeightInfo = currentNode->descendantWithHeightInfo[descendantHeight];
                auto& numPairsWithHeightWithNodeAsLCA = descendantHeightInfo.numPairsWithHeightWithNodeAsLCA;
                auto& numberDescendantsWithThisHeight = descendantHeightInfo.number;

                if (numUnprocessedDescendantsWithHeight * numPairsWithHeightWithNodeAsLCA > 0)
                {
                    // Found a triple where all three nodes have currentNode as their LCA: a "Type B" triple.
                    const auto numNewTriplets = numPairsWithHeightWithNodeAsLCA * numUnprocessedDescendantsWithHeight * numTripletPermutations;
                    assert(numNewTriplets >= 0);
                    numTriplets += numNewTriplets;
                }

                // These numPairsWithHeightWithNodeAsLCA would, when combined with a non-ancestor of currentNode that isSuitable and is
                // (descendantHeight - currentNode->height) distance away from currentNode, form a "Type A" triple.
                // We store numPairsWithHeightWithNodeAsLCA for this descendantHeight inside currentNode: the required non-ancestors of
                // currentNode will be found by completeTripletsOfTypeA() later on.
                numPairsWithHeightWithNodeAsLCA += numUnprocessedDescendantsWithHeight * numKnownDescendantsWithHeight;

                if (numberDescendantsWithThisHeight == 0)
                {
                    // This hasn't been updated yet, so has missed the numKnownDescendantsWithHeight; incorporate it now.
                    numberDescendantsWithThisHeight += numKnownDescendantsWithHeight;
                }
                numberDescendantsWithThisHeight += numUnprocessedDescendantsWithHeight;

            }

            // "Copy" the transient info into persistent info, and make a note that this HeightInfo has been incorporated
            // into currentNode.
            heightInfoForNode.numWithHeight += transientHeightInfo.numWithHeight;
            heightInfoForNode.lastIncorporatedIntoNode = currentNode;
        }
    }

    if (currentNode->isSuitable)
    {
        persistentInfoForDescendantHeight[currentNode->height].numWithHeight++;
        persistentInfoForDescendantHeight[currentNode->height].lastIncorporatedIntoNode = currentNode;
    }

    return persistentInfoForDescendantHeight;
}

int64_t findNumTriplets(vector<Node>& nodes)
{
    int64_t numTriplets = 0;

    auto rootNode = &(nodes.front());

    // Fills in numPairsWithHeightWithNodeAsLCA for each node, and 
    // additionally counts all "Type B" triples and adds them to results.
    buildDescendantHeightInfo(rootNode, nullptr, 0, numTriplets);

    // Finishes off the computation of the number of "Type A" triples
    // that we began in buildDescendantHeightInfo.
    completeTripletsOfTypeACentroidDecomposition(nodes, rootNode, numTriplets);

    return numTriplets;
}

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

int main(int argc, char* argv[])
{
    // This solution is the first one that occurred to me (well, ish - the first one
    // didn't use Centroid Decomposition but a technique with similar capabilities
    // based around Heavy-Light Decomposition which seems a little faster in practice,
    // and was a little simpler for this Problem).  Based on the solutions of the Tester
    // (and basically everyone who got 100pts on this Problem during the Contest :)),
    // it's become clear that this is an unsually clunky and fiddly way of doing things,
    // so I'm only going to give a brief overview of it, here - the official Editorial 
    // solution will doubtless be better to learn from :) This explanation is adapted from
    // the "Brief (ha!) Description" that is required when submitting a new Problem Idea.
    //
    // Anyway, if you're still reading (which you shouldn't be :p), here goes:
    //
    // Imagine we pick some arbitrary node (I pick node number 1) as the root R of the tree
    // and do a DFS from there.  Then it can be shown that any triple (p, q, r) will be
    // of one of the following two types:
    //
    //
    // Type A - p, q, r have the same lca, from which they are equidistant e.g.
    //
    //                        R
    //                       /|\
    //                        ...
    //                           X = lca(p, q, r)
    //                         / | \
    //                        .......
    //                       /   |   \
    //                      p    q    r   <-- dist(p, x) = dist(q, x) = dist(r, x); 
    //                                        p, q and r all have same distance from X,
    //                                        and so the same distance from R (aka "height").
    //
    // Type B - the two "lowest" nodes - say q and r wlog, are equidistant from their lca = X.  
    // p is *not* a descendent of X, but dist(p, X) = dist(q, X) (= dist(r, X)) e.g.
    //
    //                        R
    //                       /|\
    //                        ...
    //                       /\
    //                      ... ...
    //                     /    \
    //                    p     ...
    //                           |
    //                           X = lca(q, r)
    //                         /   \
    //                        .......
    //                       /       \
    //                      q         r   <-- dist(p, X) = dist(r, X); p, q have the same distance from R.
    //
    // Type A is the easiest to compute; Type B is harder and is computed in two phases: the 
    // first phase is shared with the computation of the number of Type A triplets; the 
    // second is separate and uses Centroid Decomposition.
    // 
    // There's a reasonably well known algorithm for calculating, for each node v, the set of 
    // all descendents of v in O(N log N):
    // 
    //    findDescendents(root)
    //        set_of_descendents = empty-set
    //        for each child c of root:
    //            set_of_descendents_of_child = findDescendents(c)
    //            if |set_of_descendents_of_child| > |set_of_descendents|:
    //                swap set_of_descendents with set_of_descendents_of_child (O(1)).
    //    
    //            for each node in set_of_descendents_of_child:
    //                add node to set_of_descendents
    //    
    //        return set_of_descendents
    // 
    // The algorithm looks like it's O(N^2) worst case, but the fact that we always "copy" 
    // the smaller set into the larger actually makes it asymptotically better (O(N log N)), 
    // for the same reason as disjoint-union-by-size is O(N log N): 
    // https://en.wikipedia.org/wiki/Disjoint-set_data_structure#by_size
    // I've also seen this technique referred to as the "Small-To-Large Trick".
    // 
    // For a node v, let height(v) = dist(R, v).  We adapt the algorithm to compute not the 
    // set of descendents of each node v, but a count (map) of *heights of descendents of 
    // suitable nodes of v* i.e. a map where the keys are heights and the values are the 
    // number of descendents of v which are suitable and have that height; it can be shown 
    // that this also is achievable in worst case O(N log N).
    // 
    // With some more book-keeping, it can be shown that we can extract, in O(N log N) time 
    // and space, for all nodes v, for all heights h [NB: obviously such heights are stored 
    // sparsely, else the space requirements would be O(N^2)]:
    // 
    //     the number of pairs of descendents (u', v') of v such that:
    //         * u' and v' are both suitable;
    //         * lca(u', v') = v; and
    //         * height(u') = height(v') = h (and so: dist(u', v) = dist(v', v))
    // 
    // (this is stored as descendantWithHeightInfo.numPairsWithHeightWithNodeAsLCA) and also
    // 
    //     the number of triples (u', v', w') of v such that
    //         * u', v' and w' are all suitable;
    //         * lca(u', v') = lca(u', w') = lca(v', w') = v; and
    //         * height(u') = height(v') = height(w') = h (and so: 
    //           dist(u', v) = dist(v', v) = dist(w', v))
    // 
    // This completes Phase one of two.
    // 
    // We see that the latter count of triples is the number of triples of Type A (more 
    // precisely - it is the number of such triples divided by the number of permutations 
    // of a triple, 3! - see later).
    // 
    // The former is a step towards the computation of the number of Type B triples - 
    // referring to the definition of Type B, we've found, for each X, the number of q's 
    // and r's with a given height h (which equates to a distance d from X: height(q) - height(x))
    // and lca X, and now we just need to find for each X and h the number of p's such that 
    // dist(p, X) = height(q) - height(X).  We do this latter step via Centroid Decomposition.
    // 
    // More precisely, Phase one gives us, for each node v, a map from heights to number 
    // of pairs of descendents called v.numPairsWithHeightWithNodeAsLCA - all in O(N log N) 
    // time and space.
    // 
    // We then perform Centroid Decomposition with an efficient DistTracker class that 
    // implements the following API:
    // 
    //     * addDistance(newDistance) - adds the distance to the list of tracked distances
    //                                  in O(1).
    //     * addToAllDistances(distanceIncrease) - adds distanceIncrease to each of the 
    //                                             tracked distances in O(1).
    //     * getNumWithDistance(distance) - return the number of tracked distances whos 
    //                                      current value is precisely distance in O(1).
    // 
    // A simple combination of CD and DistTracker allows us to finally "complete" the "q's" 
    // and "r's" found in Phase one with the "p's" necessary to form a complete, Type B-triplet 
    // in O(N x (log N) x (log N)).
    // 
    // It can be further shown that if a triplet (p, q, r) of either type A or B is 
    // counted by the algorithm, then none of its (3! - 1) other permutations - (p, r, q); 
    // (r, p, q) etc - will be counted, so simply multiplying the count of Type A and 
    // Type B triplets by 3! gives us the final result.
    // 
    // Well - not quite :) The Centroid Decomposition step doesn't know about parent and 
    // children in our original DFS from R, so it will overcount triples.  For example, 
    // consider the simple example:
    //
    //                     R
    //                     |
    //                     X
    //                    / \
    //                   Y   Z
    //
    // Imagine that Y and Z are the only suitable nodes, so there are no valid triples in 
    // this example.  
    //
    // Note that X's descendantWithHeightInfo[2].numPairsWithHeightWithNodeAsLCA will be 1.  
    // The Centroid Decomposition step will then, unfortunately, treat Y as a "completer" 
    // of this pair (Y, Z) (and will treat Z the same way), resulting in it reporting *2*
    // triples, instead of the correct answer of 0.  Luckily, it's easy (though clunky) to 
    // remove this overcount: see DescendantWithHeightInfo::number for more information.  
    // The proper Editorial solution, and the original HLD-based solution, don't have this 
    // clunky "overcount correction" step, which is another reason why they are superior :)
    //
    // The whole algorithm runs in O(N x (log N) x (log N)) time with O(N log N) space.
    ios::sync_with_stdio(false);

    const auto numTestcases = read<int>();

    for (auto t = 0; t < numTestcases; t++)
    {
        const auto numNodes = read<int>();
        assert(1 <= numNodes && numNodes <= 200'000);

        vector<Node> nodes(numNodes);

        for (auto i = 0; i < numNodes - 1; i++)
        {
            const auto u = read<int>();
            const auto v = read<int>();

            assert(1 <= u && u <= numNodes);
            assert(1 <= v && v <= numNodes);

            nodes[u - 1].neighbours.push_back(&(nodes[v - 1]));
            nodes[v - 1].neighbours.push_back(&(nodes[u - 1]));
        }
        for (auto i = 0; i < numNodes; i++)
        {
            const auto isSuitable = read<int>();

            assert(isSuitable == 0 || isSuitable == 1);

            nodes[i].isSuitable = (isSuitable == 1);
        }

        const auto numTriplets = findNumTriplets(nodes);
        cout << numTriplets << endl;
    }

    assert(cin);
}
