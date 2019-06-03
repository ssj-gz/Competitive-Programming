#include <iostream>
#include <vector>
#include <map>
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
            //cout << "index: " << index << " m_minIndex: " << m_minIndex << endl;
            assert(index >= m_minIndex);
            assert(index <= m_maxIndex);
            assert(index - m_minIndex < m_vector.size());
            return m_vector[index - m_minIndex];
        }
        const T& operator[](int index) const
        {
            //cout << "index: " << index << " m_minIndex: " << m_minIndex << endl;
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
    vector<Vec<D>> things;
    Vec<D> initial(0, 0);
    initial[0].numAdditions = 0;
    things.push_back(initial);
    for (const auto pair : numWithAbsColorDiff)
    {
        const int absColorDiff = pair.first;
        const int numWithAbsColorDiff = pair.second;
        distinctAbsDiffs.push_back(absColorDiff);
        maxPossibleDiff += absColorDiff * numWithAbsColorDiff;
        cout << endl << "absColorDiff: " << absColorDiff << " numWithAbsColorDiff: " << numWithAbsColorDiff << " maxPossibleDiff: " << maxPossibleDiff << endl;
        const Vec<D>& previous = things.back();
        Vec<D> next(-maxPossibleDiff, maxPossibleDiff);

        const int start = ((numWithAbsColorDiff % 2) == 0) ? 0 : absColorDiff;
        const int end = numWithAbsColorDiff * absColorDiff;
        // What diffs can we form by adding multiples of this absColorDiff?
        cout << "Marking forwards - previous.minIndex(): " << previous.minIndex() << " previous.maxIndex(): " << previous.maxIndex() << endl;

        for (int i = previous.maxIndex(); i >= previous.minIndex(); i--)
        {
            cout << "i: " << i << " previous.minIndex: " << previous.minIndex() << " previous.maxIndex(): " << previous.maxIndex() <<  endl;
            if (previous[i].numAdditions != -1)
            {
                const int startNumAdditons = (numWithAbsColorDiff + 1) / 2;
                int numAdditions = startNumAdditons;
                for (int j = i + start; j <= i + end; j += 2 * absColorDiff)
                {
                    cout << "forwards j: " << j << " next min: " << next.minIndex() << " next max: " << next.maxIndex() << " numAdditions: " << numAdditions << " startNumAdditons: " << startNumAdditons << endl;
                    if (next[j].markedForward)
                        break;
                    cout << " forwards marked j = " << j << " as " << numAdditions << " absColorDiff:" << absColorDiff << " numWithAbsColorDiff: " << numWithAbsColorDiff << " start: " << start << " end: " << end << " startNumAdditons: " << startNumAdditons << endl;
                    next[j].numAdditions = numAdditions;
                    next[j].markedForward = true;
                    assert(numAdditions != -1);
                    assert(numAdditions <= numWithAbsColorDiff);
                    const int numSubtractions = numWithAbsColorDiff - numAdditions;
                    assert(i + numAdditions * absColorDiff - numSubtractions * absColorDiff == j);
                    numAdditions++;
                }
            }

        }
        cout << "Marking backwards" << endl;
        // What diffs can we form by subtracting multiples of this absColorDiff?
        for (int i = previous.minIndex(); i <= previous.maxIndex(); i++)
        {
            cout << "i: " << i << " previous.minIndex: " << previous.minIndex() << " previous.maxIndex(): " << previous.maxIndex() <<  endl;
            if (previous[i].numAdditions != -1)
            {
                const int startNumSubtractions = (numWithAbsColorDiff + 1) / 2;
                int numAdditions = numWithAbsColorDiff - startNumSubtractions;
                for (int j = i - start; j >= i - end; j -= 2 * absColorDiff)
                {
                    cout << "backwards j: " << j << " next min: " << next.minIndex() << " next max: " << next.maxIndex() << " numAdditions: " << numAdditions << " startNumSubtractions: " << startNumSubtractions << " start: " << start << " end: " << end << " i - end: " << (i - end) << endl;
                    if (next[j].markedBackward)
                        break;
                    cout << " backwards marked j = " << j << " as " << numAdditions << " absColorDiff:" << absColorDiff << " numWithAbsColorDiff: " << numWithAbsColorDiff << " start: " << start << " end: " << end << endl;
                    next[j].numAdditions = numAdditions;
                    next[j].markedBackward = true;
                    assert(numAdditions != -1);
                    assert(numAdditions <= numWithAbsColorDiff);
                    assert(numAdditions >= 0);
                    const int numSubtractions = numWithAbsColorDiff - numAdditions;
                    assert(i + numAdditions * absColorDiff - numSubtractions * absColorDiff == j);
                    numAdditions--;
                }
            }

        }
        for (int i = 0; i <= next.maxIndex(); i++)
        {
            assert((next[i].numAdditions == -1) == (next[-i].numAdditions == -1));
        }
        things.push_back(next);

    }


    destNumAdditionsOfEachAbsDiff.resize(*max_element(absDiffs.begin(), absDiffs.end()) + 1);


    const auto& last = things.back();
    int diffWithMinAbs = numeric_limits<int>::max();
    //int numAdditionsOfThisAbsDiff = -1;
    cout << "Extracting minAbsDiff: " << endl;
    // Go backwards so that we prefer positive sums (we can form -x for x >= 0 if and only if we can
    // form +x).
    for (int i = last.maxIndex(); i >= last.minIndex(); i--)
    {
        cout << "i: " << i << " last[i].numAdditions: " << last[i].numAdditions << endl;
        assert((last[i].numAdditions == -1) == (last[-i].numAdditions == -1));
        if (last[i].numAdditions != -1)
        {
            if (abs(i) < abs(diffWithMinAbs))
            {
                diffWithMinAbs = i;
            }
            //numAdditionsOfThisAbsDiff = last[i].numAdditions;
        }
    }
    cout << "** diffWithMinAbs: " << diffWithMinAbs << endl;
    assert(diffWithMinAbs >= 0);
#if 1
    cout << "Reconstructing - distinctAbsDiffs.size():" << distinctAbsDiffs.size() << endl;

    int generatedDiff = diffWithMinAbs;
    while (!things.empty())
    {
        const int absDiffAddedOrSubtracted = distinctAbsDiffs.back();
        const int numAdditions = things.back()[generatedDiff].numAdditions;
        assert(numAdditions >= 0);
        const int numSubtractions = numWithAbsColorDiff[absDiffAddedOrSubtracted] - numAdditions;
        const int addedToPrevious = absDiffAddedOrSubtracted * numAdditions;
        const int subtractedFromPrevious = absDiffAddedOrSubtracted * numSubtractions;
        const int previousGeneratedDiff = generatedDiff - addedToPrevious + subtractedFromPrevious;
        cout << "loop - generatedDiff: " << generatedDiff << " absDiffAddedOrSubtracted:" << absDiffAddedOrSubtracted << " numAdditionsOfThisAbsDiff: " << numAdditions << " numSubtractions: " << numSubtractions << " addedToPrevious:" << addedToPrevious << " subtractedFromPrevious: " << subtractedFromPrevious << " previousGeneratedDiff: " << previousGeneratedDiff << endl;

        destNumAdditionsOfEachAbsDiff[absDiffAddedOrSubtracted] = numAdditions;

        generatedDiff = previousGeneratedDiff;
        things.pop_back();
        distinctAbsDiffs.pop_back();
    }

    {
        cout << "Verifying" << endl;
        int sum = 0;
        for (int absDiff = 0; absDiff < destNumAdditionsOfEachAbsDiff.size(); absDiff++)
        {
            cout << " absDiff: " << absDiff << " destNumAdditionsOfEachAbsDiff: " << destNumAdditionsOfEachAbsDiff[absDiff] << endl;
            assert(destNumAdditionsOfEachAbsDiff[absDiff] <= numWithAbsColorDiff[absDiff]);
            sum += destNumAdditionsOfEachAbsDiff[absDiff] * absDiff;
            sum -= (numWithAbsColorDiff[absDiff] - destNumAdditionsOfEachAbsDiff[absDiff]) * absDiff;
        }
        cout << "Verify: sum: " << sum << " diffWithMinAbs: " << diffWithMinAbs << endl;
        assert(sum == diffWithMinAbs);
    }
#endif


    return abs(diffWithMinAbs);
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);

    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int N = rand() % 30 + 1;
        const int M = rand() % (N * (N - 1) / 2 + 1);
        cout << N << " " << M << endl;

        vector<string> blah;
        for (int i = 1; i <= N; i++)
        {
            for (int j = i + 1; j <= N; j++)
            {
                blah.push_back(to_string(i) + " " + to_string(j));
            }
        }
        assert(blah.size() >= M);

        random_shuffle(blah.begin(), blah.end());

        for(int i = 0; i < M; i++)
        {
            cout << blah[i] << endl;
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

    vector<Component> components;
    for (Node& node : nodes)
    {
        if (node.componentNum == -1)
        {
            Component newComponent;
            newComponent.rootNode = &node;
            const int componentNum = components.size();
            newComponent.id = componentNum;
            cout << "found new component beginning at : " << node.id << endl;


            vector<Node*> nodesToExplore = { &node };
            node.visitScheduled = true;
            int depth = 0;
            while (!nodesToExplore.empty())
            {
                cout << "Iteration - depth: " << depth << endl;
                Node::ColorCategory colorCategory = ((depth % 2) == 0) ? Node::SameAsRoot : Node::DifferentFromRoot;
                for (Node* nodeToExplore : nodesToExplore)
                {
                    cout << "nodeToExplore: " << nodeToExplore->id << " colorCategory: " << nodeToExplore->colorCategory << endl;
                    if (nodeToExplore->colorCategory != Node::Unknown)
                    {
                        assert(nodeToExplore->colorCategory == colorCategory);
                        continue;
                    }
                    nodeToExplore->colorCategory = colorCategory;
                    nodeToExplore->componentNum = componentNum;
                    cout << " set node: " << nodeToExplore->id << " to colorCategory: " << colorCategory << endl;
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
                        cout << " adding neighbour: " << neighbour->id << " of " << nodeToExplore->id << endl;
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

    bool allComponentsHaveSizeOne = true;
    vector<int> absDiffs;
    for (const auto& component : components)
    {
        cout << "component: absColorDiff: " << component.absColorDiff <<  endl;
        if (component.numNodes != 1)
        {
            allComponentsHaveSizeOne = false;
        }
        absDiffs.push_back(component.absColorDiff);

        for (const auto& node : component.nodes)
        {
            assert(node->colorCategory != Node::Unknown);
            cout << node->id << " " << (node->colorCategory == Node::SameAsRoot ? "Same colorCategory as root" : "Different colorCategory from root") << endl;
        }
    }

    const auto minAbsDiff_BruteForce = minAbsDiffBruteForce(absDiffs);
    vector<int> numAdditionsOfEachAbsDiff;
    const auto minAbsDiff_Optimised = minAbsDiffOptimsed(absDiffs, numAdditionsOfEachAbsDiff);

    cout << "minAbsDiff_BruteForce: " << minAbsDiff_BruteForce << " minAbsDiff_Optimised: " << minAbsDiff_Optimised << endl;
    assert(minAbsDiff_Optimised == minAbsDiff_BruteForce);

    // Apply the resulting numAdditionsOfEachAbsDiff that give the minimum sum
    // by choosing the actual colour of the root node of each component (and so,
    // the colours of all nodes in that component) appropriately.
    //
    // Arbitrarily, we ensure that the number of black nodes is at least equal to
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
            // Need to make this contribute *positively* to the diff i.e. need more
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
            // Need to make this contribute *negatively* to the diff i.e. need more
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
            cout << "Node: " << node.id << " is white" << endl;
            numWhiteNodes++;
        }
        else
        {
            cout << "Node: " << node.id << " is black" << endl;
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
        cout << "Have componentWithMoreThanOneNode" << endl;
        Node* rootNode = componentWithMoreThanOneNode->rootNode;
        Node* rootNodeNeighbour = rootNode->neighbours.front();
        Node* whiteNode = rootNode;
        Node* blackNode = rootNodeNeighbour;
        const int whiteOrBlackNodeComponentId = componentWithMoreThanOneNode->id;
        if (rootNode->color != Node::Color::White)
        {
            swap(whiteNode, blackNode);
        }
        cout << "whiteNode: " << whiteNode->id << " blackNode: " << blackNode->id << endl;
        assert(whiteNode->color == Node::Color::White && blackNode->color == Node::Color::Black);
        for(auto& component : components)
        {
            if (component.id == whiteOrBlackNodeComponentId)
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
        cout << "Do not have component with more than one node" << endl;
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
        cout << "whiteNode: " << whiteNode->id << " blackNode: " << blackNode->id << endl;
        for (auto& node : nodes)
        {
            if (node.color == Node::Color::White)
            {
                if (&node != whiteNode) // Prevent double-links between whiteNode and blackNode.
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
    {
        // Verify.
        cout << "Verifying construction of graph" << endl;
        assert(numBlackNodes - numWhiteNodes == minAbsDiff_Optimised);
        cout << "nodesToConnect.size():" << nodesToConnect.size() << endl;
#if 0
        for (const auto nodePair : nodesToConnect)
        {
            cout << " adding edge: " << nodePair.first->id << " - " << nodePair.second->id << endl;
            nodePair.first->neighbours.push_back(nodePair.second);
            nodePair.second->neighbours.push_back(nodePair.first);
            assert(nodePair.first->color != nodePair.second->color);
            assert(nodePair.first->color != nodePair.second->color);
        }
        cout << "added new edges" << endl;
#endif
        for (auto& node : nodes)
        {
            node.visitScheduled = false;
        }
        for (auto& node : nodes)
        {
            if (node.visitScheduled)
                continue;

            vector<Node*> nodesToExplore = { &node };
            node.visitScheduled = true;
            int depth = 0;
            Node::Color expectedColor = node.color;

            while (!nodesToExplore.empty())
            {
                cout << "Iteration - depth: " << depth << endl;
                for (Node* nodeToExplore : nodesToExplore)
                {
                    cout << "nodeToExplore: " << nodeToExplore->id << " color: " << nodeToExplore->color << " expected: " << expectedColor << endl;
                    assert(nodeToExplore->color == expectedColor);
                }
                vector<Node*> nextNodesToExplore;
                for (Node* nodeToExplore : nodesToExplore)
                {
                    for (Node* neighbour : nodeToExplore->neighbours)
                    {
                        cout << " adding neighbour: " << neighbour->id << " of " << nodeToExplore->id << endl;
                        if (!neighbour->visitScheduled)
                        {
                            nextNodesToExplore.push_back(neighbour);
                            neighbour->visitScheduled = true;
                        }
                    }
                }
                nodesToExplore = nextNodesToExplore;
                depth++;
                expectedColor = (expectedColor == Node::Color::White) ? Node::Color::Black : Node::Color::White;
            }
        }
    }


    cout << nodesToConnect.size() << endl;
    for (const auto nodePair : nodesToConnect)
    {
        cout << nodePair.first->id << " " << nodePair.second->id << endl;
    }
}
