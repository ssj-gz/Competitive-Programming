// Simon St James (ssjgz) - 2019-10-06
// 
// Solution to: https://www.codechef.com/OCT19A/problems/BACREP
//
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

// Typical SegmentTree - you can find similar implementations all over the place :)
class NodeIdTracker
{
    public:
        NodeIdTracker() = default;
        NodeIdTracker(int maxId)
            : m_maxId{maxId},
            m_numElements{2 * maxId},
            m_elements(m_numElements + 1)
            {
            }
        int numIdsGreaterThan(int nodeId)
        {
            return numInRange(nodeId + 1, m_maxId);
        }
        int64_t numIdsLessThan(int nodeId)
        {
            return numInRange(0, nodeId - 1);
        }

        void registerNodeId(int nodeId, int64_t blop)
        {
            const auto n = m_numElements;
            auto elements = m_elements.data();
            int pos = nodeId + 1; // Make 1-relative.
            while(pos <= n)
            {
                elements[pos] += blop;
                assert(elements[pos] >= 0);
                pos += (pos & (pos * -1));
            }
        }
    private:
        int m_maxId;
        int m_numElements;
        vector<int64_t> m_elements;

        // Find the number in the given range (inclusive) in O(log2(maxId)).
        int numInRange(int start, int end) const
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
};

struct Query;
struct AddEvent
{
    Query* originalQuery = nullptr;
    int time = -1;
    int64_t numBacteriaToAdd = -1;
};

struct QueryEvent
{
    Query* originalQuery = nullptr;
    int time = -1;
};

struct Node
{
    vector<Node*> neighbours;
    vector<Node*> children;
    int depth = -1;
    int64_t initialBacteria = -1;
    int nodeId = -1;

    int64_t numBacteriaAtSecondBegin = -1;
    int64_t numBacteriaAtSecondEnd = -1;

    vector<AddEvent> addEvents;
    vector<QueryEvent> queryEvents;
};

struct Query
{
    bool isAddBacteria = false;
    int64_t numBacteriaToAdd = -1;
    int nodeId = -1;

    int64_t queryAnswer = -1;
};

void fixParentAndChild(Node* node, int depth, Node* parent)
{
    node->depth = depth;
    for (auto child : node->neighbours)
    {
        if (child == parent)
            continue;
        node->children.push_back(child);
        fixParentAndChild(child, depth + 1, node);
    }
}

vector<int64_t> solveBruteForce(vector<Node>& nodes, const vector<Query>& queries)
{
    vector<int64_t> result;

    for (auto& node : nodes)
    {
        node.numBacteriaAtSecondEnd = node.initialBacteria;
    }

    for (int i = 0; i < queries.size(); i++)
    {
        // Update existing.
        for (auto& node : nodes)
        {
            node.numBacteriaAtSecondBegin = node.numBacteriaAtSecondEnd;
            if (!node.children.empty())
                node.numBacteriaAtSecondEnd = 0;
        }
        for (auto& node : nodes)
        {
            if (node.children.empty())
                continue;
            for (auto child : node.children)
            {
                child->numBacteriaAtSecondEnd += node.numBacteriaAtSecondBegin;
            }

        }
        

        // Add new.
        if (queries[i].isAddBacteria)
        {
            nodes[queries[i].nodeId - 1].numBacteriaAtSecondEnd += queries[i].numBacteriaToAdd;
        }

        // Process "how many" query.
        if (!queries[i].isAddBacteria)
        {
            result.push_back(nodes[queries[i].nodeId - 1].numBacteriaAtSecondEnd);
        }

        cout << "Num bacteria at end of second:" << endl;
        for (const auto& node : nodes)
        {
            cout << " " << node.numBacteriaAtSecondEnd;
        }
        cout << endl;
    }
    
    return result;
}

void solutionOptimisedAux(Node* node, vector<Node*>& ancestors)
{
    ancestors.push_back(node);

    for (const auto& queryEvent : node->queryEvents)
    {
        const int timeDepthDiff = node->depth - queryEvent.time;
        for (const auto& ancestor : ancestors)
        {
            for (const auto& ancestorAddEvent : ancestor->addEvents)
            {
                const auto ancestorAddDepthTimeDiff = ancestor->depth - ancestorAddEvent.time;
                const bool isLeaf = node->children.empty();
                queryEvent.originalQuery->queryAnswer = 0;
                if (isLeaf)
                {
                    if (ancestorAddDepthTimeDiff >= timeDepthDiff)
                    {
                        queryEvent.originalQuery->queryAnswer += ancestorAddEvent.numBacteriaToAdd;

                    }
                }
                else
                {
                    if (ancestorAddDepthTimeDiff == timeDepthDiff)
                    {
                        queryEvent.originalQuery->queryAnswer = ancestorAddEvent.numBacteriaToAdd;
                    }
                }
            }
        }
    }

    for (auto child : node->children)
    {
        solutionOptimisedAux(child, ancestors);
    }

    ancestors.pop_back();
}

vector<int64_t> solveOptimised(vector<Node>& nodes, vector<Query>& queries)
{
    
    for (auto& node : nodes)
    {
        node.addEvents.push_back({nullptr, -1, node.initialBacteria});
    }

    for (int time = 0; time < queries.size(); time++)
    {
        auto node = &(nodes[queries[time].nodeId - 1]);
        if (queries[time].isAddBacteria)
        {
            node->addEvents.push_back({&(queries[time]), time, queries[time].numBacteriaToAdd});
        }
        else
        {
            node->queryEvents.push_back({&(queries[time]), time});
        }
    }

    vector<Node*> ancestors;
    solutionOptimisedAux(&(nodes.front()), ancestors);

    vector<int64_t> result;
    for (const auto& query : queries)
    {
        if (!query.isAddBacteria)
        {
            assert(query.queryAnswer != -1);
            result.push_back(query.queryAnswer);
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
        const int N = rand() % 100 + 1;
        const int Q = rand() % 1000 + 1;

        cout << N << " " << Q << endl;

        for (int edge = 0; edge < N - 1; edge++)
        {
            cout << (edge + 2) << " " << (rand() % (edge + 1)) + 1 << endl;
        }

        for (int i = 0; i < N; i++)
        {
            cout << ((rand() % 1000)) << " ";
        }
        cout << endl;

        for (int i = 0; i < Q; i++)
        {
            const bool isAddBacteria = rand() % 2;
            const int nodeId = (rand() % N) + 1;
            if (isAddBacteria)
            {
                cout << "+ " << nodeId << " " << (rand() % 1000) << endl;
            }
            else
            {
                cout << "? " << nodeId << endl;
            }
        }

        return 0;
    }
    struct timeval time;
    gettimeofday(&time,NULL);
    srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

#if 0
    const int maxID = 500'000;
    NodeIdTracker nodeIdTracker(maxID);
    int64_t sum = 0;
    for (int i = 0; i < 500'000; i++)
    {

        nodeIdTracker.registerNodeId(rand() % maxID, rand() % 1000);
        const int blah = rand() % maxID;
        const auto blee = nodeIdTracker.numIdsLessThan(blah);
        const auto bloo = nodeIdTracker.numIdsGreaterThan(rand() % maxID);
        sum += blee + bloo;
        //cout << "i: " << i << " blah: " << blah << " blee: " << blee << endl;

    }
    cout << "Done!" << sum << endl;
    return 0;
#endif
    const int N = read<int>();
    const int Q = read<int>();

    vector<Node> nodes(N);

    for (int i = 0; i < N - 1; i++)
    {
        const int x = read<int>() - 1;
        const int y = read<int>() - 1;

        nodes[x].neighbours.push_back(&(nodes[y]));
        nodes[y].neighbours.push_back(&(nodes[x]));
    }

    for (int i = 0; i < N; i++)
    {
        nodes[i].nodeId = i + 1;
        nodes[i].initialBacteria = read<int64_t>();
    }

    vector<Query> queries(Q);

    for (int i = 0; i < Q; i++)
    {
        const char queryType = read<char>();
        const int nodeId = read<int>();

        queries[i].nodeId = nodeId;

        if (queryType == '+')
        {
            queries[i].isAddBacteria = true;
            queries[i].numBacteriaToAdd = read<int64_t>();
        }
    }

    fixParentAndChild(&(nodes.front()), 0, nullptr);

#ifdef BRUTE_FORCE
    const auto solutionBruteForce = solveBruteForce(nodes, queries);
    cout << "solutionBruteForce: " << endl;
    for (const auto x : solutionBruteForce)
    {
        cout << x << endl;
    }
    const auto solutionOptimised = solveOptimised(nodes, queries);
    cout << "solutionOptimised:  " << endl;
    for (const auto x : solutionOptimised)
    {
        cout << x << endl;
    }

#if 0
    assert(solutionOptimised == solutionBruteForce);
#endif
#else
    const auto solutionOptimised = solveOptimised();
    cout << solutionOptimised << endl;
#endif

    assert(cin);
}
