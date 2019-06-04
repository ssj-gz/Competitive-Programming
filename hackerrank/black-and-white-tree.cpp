// Simon St James (ssjgz) - 2019-06-03
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <limits>
#include <algorithm>
#include <cassert>

#include <sys/time.h>

using namespace std;

struct Node
{
    int id = 0;
    vector<Node*> neighbours;
    int componentNum = -1;
    enum ColorCategory
    {
        Unknown,
        SameAsRoot,
        DifferentFromRoot
    };
    enum class Color 
    {
        Unknown,
        Black,
        White
    };
    ColorCategory colorCategory = Unknown;
    Color color = Color::Unknown;
    bool visitScheduled = false;
};

ostream& operator<<(ostream& os, Node::Color color)
{
    if (color == Node::Color::White)
    {
        os << "white";
    }
    else if (color == Node::Color::Black)
    {
        os << "black";
    }
    else
    {
        os << "unknown";
    }

    return os;
}

struct Component
{
    vector<Node*> nodes;
    Node* rootNode = nullptr;
    int numNodes = 0;
    int numNodesSameColorAsRoot = 0;
    int absColorDiff = -1;
    int id = -1;

};

/**
  * Simple vector-ish data structure that allows negative indices.
  */
template<typename T>
class Vec
{
    public:
        Vec(int minIndex, int maxIndex)
            : m_minIndex(minIndex),
            m_maxIndex(maxIndex),
            m_numElements(maxIndex - minIndex + 1),
            m_vector(m_numElements)
    {

    }
        int minIndex() const
        {
            return m_minIndex;
        }
        int maxIndex() const
        {
            return m_maxIndex;
        }
        T& operator[](int index)
        {
            assert(index >= m_minIndex);
            assert(index <= m_maxIndex);
            assert(index - m_minIndex < m_vector.size());
            return m_vector[index - m_minIndex];
        }
        const T& operator[](int index) const
        {
            assert(index >= m_minIndex);
            assert(index <= m_maxIndex);
            assert(index - m_minIndex < m_vector.size());
            return m_vector[index - m_minIndex];
        }
    private:
        int m_minIndex = -1;
        int m_maxIndex = -1;
        int m_numElements = -1;
        vector<T> m_vector;
};

struct D
{
    int numAdditions = -1;
    bool markedDuringAdditions = false;
    bool markedDuringSubtractions = false;
};

void minAbsDiffBruteForce(const vector<int>& absDiffs, int absDiffIndex, int diffSoFar, int& minAbsDiff)
{
    if (absDiffIndex == absDiffs.size())
    {
        if (abs(diffSoFar) < minAbsDiff)
        {
            minAbsDiff = min(minAbsDiff, abs(diffSoFar));
            cout << "Found new smallest minAbsDiff: " << minAbsDiff << endl;
        }

        return;
    }
    // Add this diff.
    minAbsDiffBruteForce(absDiffs, absDiffIndex + 1, diffSoFar + absDiffs[absDiffIndex], minAbsDiff);
    // Subtract this diff.
    minAbsDiffBruteForce(absDiffs, absDiffIndex + 1, diffSoFar - absDiffs[absDiffIndex], minAbsDiff);
}

int minAbsDiffBruteForce(const vector<int>& absDiffs)
{
    int minAbsDiff = numeric_limits<int>::max();
    minAbsDiffBruteForce(absDiffs, 0, 0, minAbsDiff);
    return minAbsDiff;
}

int minAbsDiffOptimsed(const vector<int>& absDiffs, vector<int>& destNumAdditionsOfEachAbsDiff)
{
    map<int, int> numWithAbsColorDiff;
    for (const auto diff : absDiffs)
    {
        numWithAbsColorDiff[diff]++;
    }
    vector<int> distinctAbsDiffs;
    int maxPossibleDiff = 0;
    // generatableDiffsForDistinctAbsDiff[i][x].numAdditions != -1 if and only if we can generate x by combinations of
    // additions or subtractions of all absDiffs <= distinctAbsDiffs[i].
    // The "numAdditions" is a backlink that helps us reconstruct *which* absDiffs should be added and which
    // subtracted in order to form the minAbsDiff.
    vector<Vec<D>> generatableDiffsForDistinctAbsDiff;
    Vec<D> initial(0, 0);
    initial[0].numAdditions = 0;
    generatableDiffsForDistinctAbsDiff.push_back(initial);
    for (const auto pair : numWithAbsColorDiff)
    {
        const int absColorDiff = pair.first;
        const int numWithAbsColorDiff = pair.second;
        distinctAbsDiffs.push_back(absColorDiff);
        maxPossibleDiff += absColorDiff * numWithAbsColorDiff;
        const Vec<D>& previous = generatableDiffsForDistinctAbsDiff.back();
        // We could use -N to +N as a range, but that might take up too much memory:
        // -maxPossibleDiff to maxPossibleDiff is a much more memory-efficient bound.
        Vec<D> next(-maxPossibleDiff, maxPossibleDiff);

        const int minNonNegativeDiff = ((numWithAbsColorDiff % 2) == 0) ? 0 : absColorDiff;
        const int maxNonNegativeDiff = numWithAbsColorDiff * absColorDiff;
        // What diffs can we form by adding multiples of this absColorDiff? ("Forward" pass)
        for (int i = previous.maxIndex(); i >= previous.minIndex(); i--)
        {
            if (previous[i].numAdditions != -1)
            {
                const int startNumAdditions = (numWithAbsColorDiff + 1) / 2;
                int numAdditions = startNumAdditions;
                for (int j = i + minNonNegativeDiff; j <= i + maxNonNegativeDiff; j += 2 * absColorDiff)
                {
                    if (next[j].markedDuringAdditions)
                    {
                        // This j - and subsequent j's - have been marked by a previous i.
                        // No need to continue.  This squashes the complexity down from O(N ^ 2) to O(N).
                        break;
                    }
                    next[j].numAdditions = numAdditions;
                    next[j].markedDuringAdditions = true;
                    assert(numAdditions >= 0 && numAdditions <= numWithAbsColorDiff);
                    const int numSubtractions = numWithAbsColorDiff - numAdditions;
                    assert(i + numAdditions * absColorDiff - numSubtractions * absColorDiff == j);
                    numAdditions++;
                }
            }

        }
        // What diffs can we form by subtracting multiples of this absColorDiff? ("Backward" pass)
        for (int i = previous.minIndex(); i <= previous.maxIndex(); i++)
        {
            if (previous[i].numAdditions != -1)
            {
                const int startNumSubtractions = (numWithAbsColorDiff + 1) / 2;
                int numAdditions = numWithAbsColorDiff - startNumSubtractions;
                for (int j = i - minNonNegativeDiff; j >= i - maxNonNegativeDiff; j -= 2 * absColorDiff)
                {
                    if (next[j].markedDuringSubtractions)
                    {
                        // This j - and subsequent j's - have been marked by a previous i.
                        // No need to continue.  This squashes the complexity down from O(N ^ 2) to O(N).
                        break;
                    }
                    next[j].numAdditions = numAdditions;
                    next[j].markedDuringSubtractions = true;
                    assert(numAdditions >= 0 && numAdditions <= numWithAbsColorDiff);
                    const int numSubtractions = numWithAbsColorDiff - numAdditions;
                    assert(i + numAdditions * absColorDiff - numSubtractions * absColorDiff == j);
                    numAdditions--;
                }
            }

        }
#ifndef NDEBUG
        for (int i = 0; i <= next.maxIndex(); i++)
        {
            // Should be able to form x if and only if we can form -x.
            assert((next[i].numAdditions == -1) == (next[-i].numAdditions == -1));
        }
#endif
        generatableDiffsForDistinctAbsDiff.push_back(next);

    }

    destNumAdditionsOfEachAbsDiff.resize(*max_element(absDiffs.begin(), absDiffs.end()) + 1);

    const auto& generatableDiffsFromAllAbsDiffs = generatableDiffsForDistinctAbsDiff.back();
    int minAbsDiff = -1;
    // We can assume that minAbsDiff is non-negative, since we can generate minAbsDiff iff we can generate
    // -minAbsDiff.
    for (int i = 0; i <= generatableDiffsFromAllAbsDiffs.maxIndex(); i++)
    {
        assert((generatableDiffsFromAllAbsDiffs[i].numAdditions == -1) == (generatableDiffsFromAllAbsDiffs[-i].numAdditions == -1));
        if (generatableDiffsFromAllAbsDiffs[i].numAdditions != -1)
        {
            minAbsDiff = i;
            break;
        }
    }
    assert(minAbsDiff >= 0);

    // Reconstruct how many of each distinctAbsDiffs we added (and subtracted) in order
    // to obtain the final minAbsDiff, and store the results in destNumAdditionsOfEachAbsDiff.
    // We work backwards, moving back through the list of distinctAbsDiffs and the corresponding
    // generatableDiffsForDistinctAbsDiff's.
    // The "Verify" block below ensures that we have a correct reconstructions and may clarify things
    // a little.
    int generatedDiff = minAbsDiff;
    while (!generatableDiffsForDistinctAbsDiff.empty())
    {
        const int absDiffAddedOrSubtracted = distinctAbsDiffs.back();
        const int numAdditions = generatableDiffsForDistinctAbsDiff.back()[generatedDiff].numAdditions;
        assert(numAdditions >= 0);
        const int numSubtractions = numWithAbsColorDiff[absDiffAddedOrSubtracted] - numAdditions;
        const int addedToPrevious = absDiffAddedOrSubtracted * numAdditions;
        const int subtractedFromPrevious = absDiffAddedOrSubtracted * numSubtractions;
        const int previousGeneratedDiff = generatedDiff - addedToPrevious + subtractedFromPrevious;

        destNumAdditionsOfEachAbsDiff[absDiffAddedOrSubtracted] = numAdditions;

        generatedDiff = previousGeneratedDiff;
        generatableDiffsForDistinctAbsDiff.pop_back();
        distinctAbsDiffs.pop_back();
    }

    {
#ifndef NDEBUG
        // Verify that our reconstruction is indeed correct.
        int sum = 0;
        for (int absDiff = 0; absDiff < destNumAdditionsOfEachAbsDiff.size(); absDiff++)
        {
            assert(destNumAdditionsOfEachAbsDiff[absDiff] <= numWithAbsColorDiff[absDiff]);
            sum += destNumAdditionsOfEachAbsDiff[absDiff] * absDiff;
            sum -= (numWithAbsColorDiff[absDiff] - destNumAdditionsOfEachAbsDiff[absDiff]) * absDiff;
        }
        assert(sum == minAbsDiff);
#endif
    }

    return minAbsDiff;
}

int main(int argc, char* argv[])
{
    // Hmmm ... this is a rather baffling one from a "history" point of view:
    // apparently, I looked at the Editorial on 24-01-18, but have no recollection
    // of doing so, but *do* recall solving each individual subproblem, though
    // perhaps these latter are phantoms.  Having reviewed the Editorial, it is 
    // unusually unhelpful, so I'm very confused as to why I would have ever looked at it.
    // Anyway - no points for this one, sadly :(
    //
    // So: how to solve? Firstly, all the graph theory stuff is kind of a red herring, or
    // rather it's by far the easiest part.
    //
    // For a given connected component of the original graph, we have very limited choice
    // about the colourings for each node: once we've picked some arbitrary "root" of the
    // component and decided on a colour for it, the colours of all the other nodes in
    // that component are pre-ordained - if the root node is chosen to be black, then its
    // immediately neighbours *must* be coloured white, and their immediate neighbours black, etc.
    // The ability to add new edges gives us absolutely no extra leeway here.
    // Let x be the number of connected components in the original graph, and C_1, C_2, ... C_x
    // the set of these components.
    //
    // It would be a little premature to give concrete colours to the root node of each C_i,
    // but we can categorise them, using the above logic, to SameAsRoot or DifferentFromRoot: a simple
    // breadth-first search is sufficient to do this.
    //
    // Note that, if we decide on a colouring of all nodes in the graph and compute D for it, 
    // then switching the colours of all nodes gives the same D (since we take the *absolute value*
    // of the difference between number of black and number of white nodes).  So let's assume,
    // quite arbitrarily, that our final constructed graph will have at least as many black nodes
    // as white (if we had a graph that did not satisfy this, we could produce one with the exact
    // same D and with at least as many black just by switching the colours of all nodes).
    // Then, D = (num black nodes - num white nodes) - no "abs" required.
    //
    // Let's quickly dismiss the rest of the graph theory part of the problem.  If we have decided on
    // a colouring of all nodes that minimises D, then we can make it connected very easily:
    // if one of the components, C_j say, has more than one node in it, then it has both a Black and a 
    // White node - pick one of each colour from C_j and call them whiteNode and blackNode, respectively. 
    // Then for every C_i != C_j, simply pick any node from C_i and add a new edge from that node to either
    // whiteNode or blackNode, as appropriate: the resulting graph is connected (using C_j as a central
    // "hub") and has different colour nodes at the ends of each edge, so is a solution to the problem.
    // There's some minor difficulties arising from graphs where none of the C_is have more than one
    // node, but these are easily dealt with.
    //
    // Back to the main problem: we've computed for each C_i a partitioning of all nodes in that C_i into SameAsRoot and 
    // DifferentFromRoot.  Now, note that if we set the root node of C_i to Black, then we contribute
    // (num SameAsRoot in C_i - num DifferentFromRoot in C_i) to D; likewise, if we set it to White,
    // we contribute (num DifferentFromRoot in C_i - num SameAsRoot in C_i) to D.  That is:
    // each component contributes either +(num SameAsRoot in C_i - num DifferentFromRoot in C_i) or
    // -(num SameAsRoot in C_i - num DifferentFromRoot in C_i) to D, depending on what colour we 
    // set the root of that node.  This (num SameAsRoot in C_i - num DifferentFromRoot in C_i) is very
    // important - let's call it absColorDiff(C_i) i.e. each C_i contributes either +absColorDiff(C_i)
    // or -absColorDiff(C_i) to D.
    //
    // Consider the set of 2^x vectors of the form (s1, s2, ... sx) where each s_i is either +1 or -1.
    // Let the "diff generated from this vector" be:
    //
    //    s1 * absColorDiff(C_1) + s2 * absColorDiff(C_2) * + ... + sx * absColorDiff(C_x)
    //
    // It's hopefully obvious that we're looking for the generated diff that has the minimum absolute value
    // of all generated diffs arising from the set of all 2^x vectors of this form.
    //
    // A quick observation: (-s1, -s2, ... -sx) will also be in tihs set of vectors, and will generate the negative
    // diff to (-s1, -s2, ... -sx): that is, we can generate d' if and only if we can generate -d'.  So wlog
    // we can assume that the generated diff with the min absolute value is non-negative.
    //
    // Having found the vector (s1, s2, ... sx) that generates this minimum non-negative diff, we can the use
    // it to finally assign a colouring to each component that gives the minimum D.  This vector is essentially 
    // represented in the code by numAdditionsOfEachAbsDiff, although it is does not take the form of a x-vector
    // of +/-1's (it instead encodes how many times each absColorDiff is *added* to the total - and so, implicitly,
    // how many times it is subtracted).
    //
    // So: 2^x vectors to try, and x ~O(N) - not very tractable :) How can we improve this?
    //
    // Well, we can immediately squish it down to polynomial time using standard dynamic programming techniques.
    //
    // Let canBeGenerated[i][d] be true if and only if we can generate d by adding positive or negative versions of
    // absColorDiff(C_1), absColorDiff(C_2) ... absColorDiff(C_i) - then  there's a simple recurrence relation:
    //
    //   for each d such that canBeGenerated[i][d], set 
    //      canBeGenerated[i+1][d + absColorDiff(C_(i+1))] to true and set
    //      canBeGenerated[i+1][d - absColorDiff(C_(i+1))] to true.
    //
    // We then just need to find the smallest d such that canBeGenerated[x][d] is true.
    //
    // Each step (from i to i+1) in the recurrence relation is O(N) (there can be O(N) such d), and x itself is O(N)
    // so this is O(N^2) - still too high.  How can we improve matters further?
    //
    // Well, we can note that a component C_i obviously has at least absColorDiff(C_i) nodes in it, and so
    //
    //   absColorDiff(C_1) + absColorDiff(C_2) + ... + absColorDiff(C_x) <= N.
    //
    // From this, we can deduce that the number of *distinct* absColorDiff(C_i)'s is O(sqrt(N)) (for the same reason
    // that if 1 + 2 + ... + k <= N, then k can be at most O(sqrt(N)) - a fairly well-known result).
    // Thus, we can boil down the set of absColorDiff(C_1) ... absColorDiff(C_x) to a set of O(sqrt(N)) distinctAbsDiffs
    // and a tally of how many copies of each distinct abs diff there are (numWithAbsColorDiff).
    //
    // Does this help? Initially, it seems not that much: if we enumerate the distinctAbsDiffs, da1, da2, ... da_l, 
    // then we can reformulate canBeGenerated[i][d] as:
    //
    //    canBeGenerated[i][d] is true if and only if d can be generated using only additions and subtractions of the diffs
    //    da1 (numWithAbsColorDiff(da1) of these), da2 (numWithAbsColorDiff(da2) of these), ... da_l (numWithAbsColorDiff(da_l)
    // of these)
    //
    // and then we can solve the problem by finding the smallest d such that canBeGenerated[l][d] is true.
    //
    // Since l is O(sqrt(N)), it looks like we have asymptotically fewer iteration i's to compute (O(N) previously; O(sqrt(N)) now),
    // so that sounds like progress! But how do we compute canBeGenerated[i+1][d] when we have canBeGenerated[i+1]?
    //
    // Well, canBeGenerated[i+1][d] is true if and only if there is some assignment of +'s and minus's to the numWithAbsColorDiff(da_(i+1))
    // da_(i+1)'s such that d minus the result is true in canBeGenerated[i+1].  But, of course, there are O(2^numWithAbsColorDiff(da_(i+1))
    // ways of doing this, and numWithAbsColorDiff(da_(i+1)) can be O(N), so this is no immediate help.
    //
    // It's hopefully obvious that the result of assigning +'s and -'s to the numWithAbsColorDiff(da_(i+1)) da_(i+1)'s depends only on
    // how many +'s and -'s there are - it is # +'s * da_(i+1) - # -'s da_(i+1).  Further, fixing # +'s automatically fixes # -'s -  
    // # -'s is numWithAbsColorDiff(da_(i+1)) - # +'s.  Thus, our recurrence relation can be re-written as:
    //
    //   for each d such that canBeGenerated[i][d],
    //      for each numAdditions = 0, 1, ... numWithAbsColorDiff(da_(i+1))
    //         set canBeGenerated[i+1][d + numAdditions * da_(i+1) - (numWithAbsColorDiff(da_(i+1) - numAdditions) * da_(i+1)]  to true
    //
    // There are O(N) such d, as before, and O(N) possible values of numAdditions, and we need to do this for O(sqrt(N)) values of i,
    // giving a total of O(sqrt(N) * N * N) computations.  Rats - this is even worse than before!
    //
    // But there is a shortcut.  Consider what happens with numAdditions as we increase or decrease it.  Let's start it off as 0 - then 
    // we are subtracting numWithAbsColorDiff(da_(i+1)) * da_(i+1)).  Let's increase numAdditions by one (and so, also reduce the number of 
    // subtractions by one) - then we *decrease* the amount we are subtracting by 2 * da_(i+1).  As we keep increasing numAdditions, we
    // decrease the amount we are subtracting(!) until, when numAdditions becomes greater than or equal to numWithAbsColorDiff(da_(i+1)) / 2,
    // we stop subtracting and either do nothing (if numWithAbsColorDiff(da_(i+1)) is even) or start adding (otherwise).
    //
    // Let minNonNegativeDiff be the smallest non-negative number that can be formed by changing the value of numAdditions - it is 
    // 0 if numWithAbsColorDiff(da_(i+1)) is even, and da_(i+1) otherwise.
    //
    // Then, we can replace the above recurrence relation by:
    //
    //   for each d such that canBeGenerated[i][d],
    //      # Mark d's forward.
    //      for each positiveDiff = minNonNegativeDiff, minNonNegativeDiff + 2 * da_(i+1),  ... numWithAbsColorDiff(da_(i+1)) * da_(i+1)
    //         canBeGenerated[i+1][d + positiveDiff] to true
    //      # Mark d's backward.
    //      for each negDiff = -minNonNegativeDiff, minNonNegativeDiff - 2 * da_(i+1), ... , -numWithAbsColorDiff(da_(i+1)) * da_(i+1)
    //         canBeGenerated[i+1][d + negDiff] to true
    //
    // Not much better. Here, we start at each d, and then make two passes: "forward" and "backward".  In the forward pass, we 
    // mark each d + minNonNegativeDiff, minNonNegativeDiff + 2 * da_(i+1), d + 4 * da_(i+1), ... , d + numWithAbsColorDiff(da_(i+1)) * da_(i+1)
    // i.e starting at d + minNonNegativeDiff, we mark each 2 * da_(i+1)th element until we reach d + numWithAbsColorDiff(da_(i+1)) * da_(i+1),
    // and similar for the "backward" pass, except we start at d - minNonNegativeDiff and mark each 2 * da_(i+1)th element going backwards.
    // This is O(N^2) over all d.
    //
    // However, imagine in the forward pass we started at the maximum value of d and went backwards to the minimum value of d.  Then, if 
    // any of the sequence d + minNonNegativeDiff, d + minNonNegativeDiff + 2 * da_(i+1) etc has already been marked by a forward pass, then we
    // don't have to mark the remainder of this sequence for this d: they would have been marked by a "previous" (i.e. - larger) d.
    // Likewise, if in a backward pass we started at the minimum value of d and went forwards, then if any of the sequence
    // d - minNonNegativeDiff, d - minNonNegativeDiff - 2 * da_(i+1) etc has been marked by a backward pass, then we know that the 
    // remainder of this sequence has already been marked by a "previous" (i.e. - smaller) d.
    //
    // This simple optimisation collapses the time taken to compute canBeGenerated[i+1] from canBeGenerated[i] from O(N^2) to O(N): 
    // to see that it is now O(N), note that the algorithm (considering only the "forward" pass here - the "backward" pass is the 
    // same by symmetry):
    //
    //    i) sweep d from its maximum value to its minimum value (contributes O(N) to computation of canBeGenerated[i+1]).
    //    ii) for each d, mark each element in the sequence d + minNonNegativeDiff, d + minNonNegativeDiff + 2 * da_(i+1) etc
    //        unless it is already marked, in which case we abort and continue with i).
    //
    // Each step in ii) must mark an unmarked element, so its total contribution across all d for canBeGenerated[i+1] is bounded
    // by the number of elements that can be marked, which is O(N).  Thus, computed canBeGenerated[i+1] is O(N), so computing
    // all canBeGenerated's is O(sqrt(N) * N) - nice and tractable :)
    //
    // That's about it - this enables us to find the minimum generatable d, and with a bit of book-keeping and memory usage we can
    // reconstruct the precise number of additions for each da_i (i = 1, 2, ... l) that gave rise to this minimum d, which
    // we store as numAdditionsOfEachAbsDiff and which we can use to give the final, concrete colouring that minimises
    // numBlackNodes - numWhiteNodes.  We then just perform the simple step of making the graph connected etc, and we're done!
    // In the code, what we've been referring to as canBeGenerated is called generatableDiffsForDistinctAbsDiff.
    ios::sync_with_stdio(false);

    if (argc == 2)
    {
        // XXX This does not guarantee a solution, but is good for verifying the 
        // important part of this problem (minAbsDiff_Optimised).
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int N = rand() % 30 + 1;
        const int M = min(rand() % (N * (N - 1) / 2 + 1), 500'000);
        cout << N << " " << M << endl;

        set<string> blah;
        while (blah.size() < M)
        {
            int node1 = (rand() % N) + 1;
            int node2 = (rand() % N) + 1;
            if (node1 == node2)
                continue;

            if (node1 > node2)
                swap(node1, node2);

            blah.insert(to_string(node1) + " " + to_string(node2));
        }
        assert(blah.size() >= M);

        //random_shuffle(blah.begin(), blah.end());

        for(const auto& edge : blah)
        {
            cout << edge << endl;
        }

        return 0;

    }
    int N, M;
    cin >> N >> M;

    vector<Node> nodes(N);
    for (int i = 0; i < M; i++)
    {
        int u, v;
        cin >> u >> v;
        u--;
        v--;

        nodes[u].neighbours.push_back(&nodes[v]);
        nodes[v].neighbours.push_back(&nodes[u]);
    }
    for (int nodeId = 1; nodeId <= N; nodeId++)
    {
        nodes[nodeId - 1].id = nodeId;
    }

    // Find the connected components, and also decide the colorCategory for each
    // node in each component using the usual Bipartite Graph algorithm.
    vector<Component> components;
    for (Node& node : nodes)
    {
        if (node.componentNum == -1)
        {
            Component newComponent;
            newComponent.rootNode = &node;
            const int componentNum = components.size();
            newComponent.id = componentNum;

            vector<Node*> nodesToExplore = { &node };
            node.visitScheduled = true;
            int depth = 0;
            while (!nodesToExplore.empty())
            {
                // Alternate the colorCategory as depth increases.
                Node::ColorCategory colorCategory = ((depth % 2) == 0) ? Node::SameAsRoot : Node::DifferentFromRoot;
                for (Node* nodeToExplore : nodesToExplore)
                {
                    if (nodeToExplore->colorCategory != Node::Unknown)
                    {
                        assert(nodeToExplore->colorCategory == colorCategory);
                        continue;
                    }
                    nodeToExplore->colorCategory = colorCategory;
                    nodeToExplore->componentNum = componentNum;
                    if (colorCategory == Node::SameAsRoot)
                    {
                        newComponent.numNodesSameColorAsRoot++;
                    }
                    newComponent.numNodes++;
                    newComponent.nodes.push_back(nodeToExplore);

                }
                vector<Node*> nextNodesToExplore;
                for (Node* nodeToExplore : nodesToExplore)
                {
                    for (Node* neighbour : nodeToExplore->neighbours)
                    {
                        if (!neighbour->visitScheduled)
                        {
                            nextNodesToExplore.push_back(neighbour);
                            neighbour->visitScheduled = true;
                        }
                    }
                }
                nodesToExplore = nextNodesToExplore;
                depth++;
            }

            newComponent.absColorDiff = abs((newComponent.numNodesSameColorAsRoot) - (newComponent.numNodes - newComponent.numNodesSameColorAsRoot));
            components.push_back(newComponent);
        }
    }

    vector<int> absDiffs;
    for (const auto& component : components)
    {
        absDiffs.push_back(component.absColorDiff);
    }

    vector<int> numAdditionsOfEachAbsDiff;
#ifdef BRUTE_FORCE
    const auto minAbsDiff_BruteForce = minAbsDiffBruteForce(absDiffs);
    const auto minAbsDiff_Optimised = minAbsDiffOptimsed(absDiffs, numAdditionsOfEachAbsDiff);

    cout << "minAbsDiff_BruteForce: " << minAbsDiff_BruteForce << " minAbsDiff_Optimised: " << minAbsDiff_Optimised << endl;
    assert(minAbsDiff_Optimised == minAbsDiff_BruteForce);
#else
    const auto minAbsDiff_Optimised = minAbsDiffOptimsed(absDiffs, numAdditionsOfEachAbsDiff);
#endif

    // Apply the resulting numAdditionsOfEachAbsDiff that give the minimum diff
    // by choosing the actual colour of the root node of each component (and so,
    // the colours of all nodes in that component) appropriately.
    //
    // Arbitrarily, we ensure that, in the final graph, the number of black nodes is at least equal to
    // the number of white nodes, and so the difference is nB - nW (i.e. no abs() needed!).
    Component* componentWithMoreThanOneNode = nullptr;
    for (auto& component : components)
    {
        Node::Color componentRootColor = Node::Color::Unknown;
        const int numSameColorAsRoot = component.numNodesSameColorAsRoot;
        const int numDifferentColorFromRoot = component.numNodes - component.numNodesSameColorAsRoot;
        if (component.numNodes > 1)
        {
            componentWithMoreThanOneNode = &component;
        }
        if (numAdditionsOfEachAbsDiff[component.absColorDiff] > 0)
        {
            // Need to make this component contribute *positively* to the diff i.e. need more
            // black nodes than white nodes!
            if (numSameColorAsRoot > numDifferentColorFromRoot)
            {
                componentRootColor = Node::Color::Black;
            }
            else
            {
                componentRootColor = Node::Color::White;
            }
            numAdditionsOfEachAbsDiff[component.absColorDiff]--;
        }
        else
        {
            // Need to make this component contribute *negatively* to the diff i.e. need more
            // white nodes than black nodes!
            if (numSameColorAsRoot > numDifferentColorFromRoot)
            {
                componentRootColor = Node::Color::White;
            }
            else
            {
                componentRootColor = Node::Color::Black;
            }
        }
        for(auto node : component.nodes)
        {
            assert(node->colorCategory != Node::Unknown);
            assert(node->color == Node::Color::Unknown);
            if (node->colorCategory == Node::SameAsRoot)
            {
                node->color = componentRootColor;
            }
            else
            {
                node->color = (componentRootColor == Node::Color::White) ? Node::Color::Black : Node::Color::White;
            }
        }
    }
    int numWhiteNodes = 0;
    int numBlackNodes = 0;
    for (auto& node : nodes)
    {
        assert(node.color != Node::Color::Unknown);
        if (node.color == Node::Color::White)
        {
            numWhiteNodes++;
        }
        else
        {
            numBlackNodes++;
        }
    }
    vector<std::pair<Node*, Node*>> nodesToConnect;
    if (numWhiteNodes == 0 || numBlackNodes == 0)
    {
        assert(N == 1);
        // Nothing to do.
    }
    else if (componentWithMoreThanOneNode)
    {
        // Pick an "official" black and white node from this component.
        // For all other components, join an arbitrary node in that component
        // to either the offical white or black node, as appropriate.
        // Graph will then be completely connected, with componentWithMoreThanOneNode 
        // acting as a "hub".
        Node* rootNode = componentWithMoreThanOneNode->rootNode;
        Node* rootNodeNeighbour = rootNode->neighbours.front();
        Node* whiteNode = rootNode;
        Node* blackNode = rootNodeNeighbour;
        if (rootNode->color != Node::Color::White)
        {
            swap(whiteNode, blackNode);
        }
        assert(whiteNode->color == Node::Color::White && blackNode->color == Node::Color::Black);
        for(auto& component : components)
        {
            if (&component == componentWithMoreThanOneNode)
            {
                continue;
            }
            auto node = component.nodes.front();
            if (node->color == Node::Color::White)
            {
                nodesToConnect.push_back({node, blackNode});
            }
            else
            {
                nodesToConnect.push_back({node, whiteNode});
            }

        }
    }
    else
    {
        // Pick a white and black node arbitrarily.
        Node* whiteNode = nullptr;
        Node* blackNode = nullptr;
        for (auto& node : nodes)
        {
            if (node.color == Node::Color::White)
            {
                whiteNode = &node;
            }
            else
            {
                blackNode = &node;
            }
        }
        assert(whiteNode != nullptr && blackNode != nullptr);
        assert(whiteNode->color == Node::Color::White && blackNode->color == Node::Color::Black);
        for (auto& node : nodes)
        {
            if (node.color == Node::Color::White)
            {
                if (&node != whiteNode) // Prevent double-links between whiteNode and blackNode - not strictly necessary, but neater!
                {
                    nodesToConnect.push_back({&node, blackNode});
                }
            }
            else
            {
                nodesToConnect.push_back({&node, whiteNode});
            }
        }
    }

    // Output the result.
    cout << minAbsDiff_Optimised << " " << nodesToConnect.size() << endl;
    for (const auto nodePair : nodesToConnect)
    {
        cout << nodePair.first->id << " " << nodePair.second->id << endl;
    }
}
