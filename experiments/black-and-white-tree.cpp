#include <iostream>
#include <vector>
#include <map>
#include <limits>
#include <cassert>

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
    int numWithDiff = 0;
};

void minAbsDiffBruteForce(const vector<int>& absDiffs, int absDiffIndex, int diffSoFar, int& minAbsDiff)
{
    if (absDiffIndex == absDiffs.size())
    {
        if (abs(diffSoFar) < minAbsDiff)
        {
            cout << "Found new smallest minAbsDiff: " << minAbsDiff << endl;
            minAbsDiff = min(minAbsDiff, abs(diffSoFar));
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

int minAbsDiffOptimsed(const vector<int>& absDiffs)
{
    map<int, int> numWithAbsColorDiff;
    for (const auto diff : absDiffs)
    {
        numWithAbsColorDiff[diff]++;
    }
    int maxPossibleDiff = 0;
    vector<Vec<D>> things;
    Vec<D> initial(0, 0);
    initial[0].numWithDiff = 1;
    things.push_back(initial);
    for (const auto pair : numWithAbsColorDiff)
    {
        const int absColorDiff = pair.first;
        const int numWithAbsColorDiff = pair.second;
        maxPossibleDiff += absColorDiff * numWithAbsColorDiff;
        cout << "absColorDiff: " << absColorDiff << " numWithAbsColorDiff: " << numWithAbsColorDiff << " maxPossibleDiff: " << maxPossibleDiff << endl;
        const Vec<D>& previous = things.back();
        Vec<D> next(-maxPossibleDiff, maxPossibleDiff);
        const int start = ((numWithAbsColorDiff % 2) == 0) ? 0 : absColorDiff;
        const int end = numWithAbsColorDiff * absColorDiff;
        // What diffs can we form by adding multiples of this absColorDiff?
        for (int i = previous.maxIndex(); i >= previous.minIndex(); i--)
        {
            cout << "i: " << i << " previous.minIndex: " << previous.minIndex() << " previous.maxIndex(): " << previous.maxIndex() <<  endl;
            if (previous[i].numWithDiff > 0)
            {
                for (int j = i + start; j <= i + end; j += 2 * absColorDiff)
                {
                    cout << "j: " << j << " next min: " << next.minIndex() << " next max: " << next.maxIndex() << endl;
                    if (next[j].numWithDiff > 0)
                        break;
                    next[j].numWithDiff = 1; // TODO - figure out better name for numWithDiff, and put correct value here.
                }
            }

        }
        // What diffs can we form by subtracting multiples of this absColorDiff?
        for (int i = previous.minIndex(); i <= previous.maxIndex(); i++)
        {
            if (previous[i].numWithDiff > 0)
            {
                for (int j = i - start; j >= i - end; j -= 2 * absColorDiff)
                {
                    if (next[j].numWithDiff > 0)
                        break;
                    next[j].numWithDiff = 1; // TODO - figure out better name for numWithDiff, and put correct value here.
                }
            }

        }

    }
    const auto& last = things.back();
    int minAbsIndex = numeric_limits<int>::max();
    for (int i = last.minIndex(); i <= last.maxIndex(); i++)
    {
        if (last[i].numWithDiff > 0)
        {
            minAbsIndex = min(minAbsIndex, abs(i));
        }
    }

    return minAbsIndex;
}

int main()
{
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
    const auto minAbsDiff_Optimised = minAbsDiffOptimsed(absDiffs);

    cout << "minAbsDiff_BruteForce: " << minAbsDiff_BruteForce << " minAbsDiff_Optimised: " << minAbsDiff_Optimised << endl;

}
