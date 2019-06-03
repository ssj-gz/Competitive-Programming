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
    enum Color
    {
        Unknown,
        SameAsRoot,
        DifferentFromRoot
    };
    Color color = Unknown;
    bool visitScheduled = false;
};

struct Component
{
    vector<Node*> nodes;
    Node* rootNode = nullptr;
    int numNodes = 0;
    int numNodesSameColorAsRoot = 0;
    int absColorDiff = -1;

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
            cout << "index: " << index << " m_minIndex: " << m_minIndex << endl;
            assert(index >= m_minIndex);
            assert(index <= m_maxIndex);
            assert(index - m_minIndex < m_vector.size());
            return m_vector[index - m_minIndex];
        }
        const T& operator[](int index) const
        {
            cout << "index: " << index << " m_minIndex: " << m_minIndex << endl;
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
                    cout << "j: " << j << " next min: " << next.minIndex() << " next max: " << next.maxIndex() << " numAdditions: " << numAdditions << " startNumAdditons: " << startNumAdditons << endl;
                    if (next[j].numAdditions != -1)
                        break;
                    cout << " forwards marked j = " << j << " as " << numAdditions << " absColorDiff:" << absColorDiff << " numWithAbsColorDiff: " << numWithAbsColorDiff << " start: " << start << " end: " << end << " startNumAdditons: " << startNumAdditons << endl;
                    next[j].numAdditions = numAdditions;
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
                    if (next[j].numAdditions != -1)
                        break;
                    cout << " backwards marked j = " << j << " as " << numAdditions << " absColorDiff:" << absColorDiff << " numWithAbsColorDiff: " << numWithAbsColorDiff << " start: " << start << " end: " << end << endl;
                    next[j].numAdditions = numAdditions;
                    assert(numAdditions != -1);
                    assert(numAdditions <= numWithAbsColorDiff);
                    assert(numAdditions >= 0);
                    const int numSubtractions = numWithAbsColorDiff - numAdditions;
                    assert(i + numAdditions * absColorDiff - numSubtractions * absColorDiff == j);
                    numAdditions--;
                }
            }

        }
        things.push_back(next);

    }


    destNumAdditionsOfEachAbsDiff.resize(*max_element(absDiffs.begin(), absDiffs.end()) + 1);

    const auto& last = things.back();
    int minAbsDiff = numeric_limits<int>::max();
    //int numAdditionsOfThisAbsDiff = -1;
    for (int i = last.minIndex(); i <= last.maxIndex(); i++)
    {
        cout << "i: " << i << " last[i].numAdditions: " << last[i].numAdditions << endl;
        if (last[i].numAdditions != -1)
        {
            minAbsDiff = min(minAbsDiff, abs(i));
            //numAdditionsOfThisAbsDiff = last[i].numAdditions;
        }
    }
    cout << "** minAbsDiff: " << minAbsDiff << endl;
#if 1
    cout << "Reconstructing - distinctAbsDiffs.size():" << distinctAbsDiffs.size() << endl;

    int generatedAbsDiff = minAbsDiff;
    while (!things.empty())
    {
        const int absDiffAddedOrSubtracted = distinctAbsDiffs.back();
        const int numAdditions = things.back()[generatedAbsDiff].numAdditions;
        const int numSubtractions = numWithAbsColorDiff[absDiffAddedOrSubtracted] - numAdditions;
        const int addedToPrevious = absDiffAddedOrSubtracted * numAdditions;
        const int subtractedFromPrevious = absDiffAddedOrSubtracted * numSubtractions;
        const int previousGeneratedAbsDiff = generatedAbsDiff - addedToPrevious + subtractedFromPrevious;
        cout << "loop - generatedAbsDiff: " << generatedAbsDiff << " absDiffAddedOrSubtracted:" << absDiffAddedOrSubtracted << " numAdditionsOfThisAbsDiff: " << numAdditions << " numSubtractions: " << numSubtractions << " addedToPrevious:" << addedToPrevious << " subtractedFromPrevious: " << subtractedFromPrevious << " previousGeneratedAbsDiff: " << previousGeneratedAbsDiff << endl;

        destNumAdditionsOfEachAbsDiff[absDiffAddedOrSubtracted] = numAdditions;

        generatedAbsDiff = previousGeneratedAbsDiff;
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
        cout << "Verify: sum: " << sum << " minAbsDiff: " << minAbsDiff << endl;
        assert(abs(sum) == minAbsDiff);
    }
#endif


    return minAbsDiff;
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
            cout << "found new component beginning at : " << node.id << endl;


            vector<Node*> nodesToExplore = { &node };
            node.visitScheduled = true;
            int depth = 0;
            while (!nodesToExplore.empty())
            {
                cout << "Iteration - depth: " << depth << endl;
                Node::Color color = ((depth % 2) == 0) ? Node::SameAsRoot : Node::DifferentFromRoot;
                for (Node* nodeToExplore : nodesToExplore)
                {
                    cout << "nodeToExplore: " << nodeToExplore->id << " color: " << nodeToExplore->color << endl;
                    if (nodeToExplore->color != Node::Unknown)
                    {
                        assert(nodeToExplore->color == color);
                        continue;
                    }
                    nodeToExplore->color = color;
                    nodeToExplore->componentNum = componentNum;
                    cout << " set node: " << nodeToExplore->id << " to color: " << color << endl;
                    if (color == Node::SameAsRoot)
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
            assert(node->color != Node::Unknown);
            cout << node->id << " " << (node->color == Node::SameAsRoot ? "Same color as root" : "Different color from root") << endl;
        }
    }

    const auto minAbsDiff_BruteForce = minAbsDiffBruteForce(absDiffs);
    vector<int> numAdditionsOfEachAbsDiff;
    const auto minAbsDiff_Optimised = minAbsDiffOptimsed(absDiffs, numAdditionsOfEachAbsDiff);

    cout << "minAbsDiff_BruteForce: " << minAbsDiff_BruteForce << " minAbsDiff_Optimised: " << minAbsDiff_Optimised << endl;
    assert(minAbsDiff_Optimised == minAbsDiff_BruteForce);

}
