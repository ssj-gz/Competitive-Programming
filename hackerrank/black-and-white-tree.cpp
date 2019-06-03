// Simon St James (ssjgz) - 2019-06-03
//#define SUBMISSION
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
    bool markedForward = false;
    bool markedBackward = false;
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
        Vec<D> next(-maxPossibleDiff, maxPossibleDiff);

        const int minNonNegativeDiff = ((numWithAbsColorDiff % 2) == 0) ? 0 : absColorDiff;
        const int maxNonNegativeDiff = numWithAbsColorDiff * absColorDiff;
        // What diffs can we form by adding multiples of this absColorDiff?
        for (int i = previous.maxIndex(); i >= previous.minIndex(); i--)
        {
            if (previous[i].numAdditions != -1)
            {
                const int startNumAdditions = (numWithAbsColorDiff + 1) / 2;
                int numAdditions = startNumAdditions;
                for (int j = i + minNonNegativeDiff; j <= i + maxNonNegativeDiff; j += 2 * absColorDiff)
                {
                    if (next[j].markedForward)
                    {
                        // This j - and subsequent j's - have been marked by a previous i.
                        // No need to continue.  This squashes the complexity down from O(N ^ 2) to O(N).
                        break;
                    }
                    next[j].numAdditions = numAdditions;
                    next[j].markedForward = true;
                    assert(numAdditions >= 0 && numAdditions <= numWithAbsColorDiff);
                    const int numSubtractions = numWithAbsColorDiff - numAdditions;
                    assert(i + numAdditions * absColorDiff - numSubtractions * absColorDiff == j);
                    numAdditions++;
                }
            }

        }
        // What diffs can we form by subtracting multiples of this absColorDiff?
        for (int i = previous.minIndex(); i <= previous.maxIndex(); i++)
        {
            if (previous[i].numAdditions != -1)
            {
                const int startNumSubtractions = (numWithAbsColorDiff + 1) / 2;
                int numAdditions = numWithAbsColorDiff - startNumSubtractions;
                for (int j = i - minNonNegativeDiff; j >= i - maxNonNegativeDiff; j -= 2 * absColorDiff)
                {
                    if (next[j].markedBackward)
                    {
                        // This j - and subsequent j's - have been marked by a previous i.
                        // No need to continue.  This squashes the complexity down from O(N ^ 2) to O(N).
                        break;
                    }
                    next[j].numAdditions = numAdditions;
                    next[j].markedBackward = true;
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

    const auto& last = generatableDiffsForDistinctAbsDiff.back();
    int minAbsDiff = -1;
    // We can assume that minAbsDiff is non-negative, since we can generate minAbsDiff iff we can generate
    // -minAbsDiff.
    for (int i = 0; i <= last.maxIndex(); i++)
    {
        assert((last[i].numAdditions == -1) == (last[-i].numAdditions == -1));
        if (last[i].numAdditions != -1)
        {
            minAbsDiff = i;
            break;
        }
    }
    assert(minAbsDiff >= 0);

    // Reconstruct how many of each distinctAbsDiffs we added (and subtracted) in order
    // to obtain the final minAbsDiff, and store the results in destNumAdditionsOfEachAbsDiff.
    // See the "Verify" block below.
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
    //cout << "minAbsDiff_Optimised: " << minAbsDiff_Optimised << endl;
    cout << minAbsDiff_Optimised << " " << nodesToConnect.size() << endl;
    for (const auto nodePair : nodesToConnect)
    {
        cout << nodePair.first->id << " " << nodePair.second->id << endl;
    }
}
