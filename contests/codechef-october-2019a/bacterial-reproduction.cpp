// Simon St James (ssjgz) - 2019-10-06
// 
// Solution to: https://www.codechef.com/OCT19A/problems/BACREP
//
#define SUBMISSION
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

void scan_char( char &x )
{
    int c = getchar_unlocked();
    for( ; (c != '+' && c != '?'); c = getchar_unlocked() );
    x = c;
}

int readInt()
{
    int toRead;
    scan_integer(toRead);
    assert(cin);
    return toRead;
}

// Typical SegmentTree - you can find similar implementations all over the place :)
class SegmentTree
{
    public:
        SegmentTree() = default;
        SegmentTree(int minPos, int maxPos)
            : m_minPos{minPos},
              m_maxPos{maxPos},
              m_numElements{2 * (maxPos - minPos + 1)},
              m_elements(m_numElements + 1)
            {
            }
        int maxPos()
        {
            return m_maxPos;
        }
        // Find the number in the given range (inclusive) in O(log2(maxId)).
        int64_t numInRange(int start, int end) const
        {
            start -= m_minPos;
            end -= m_minPos;
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

        void addValueAt(int position, int64_t toAdd)
        {
            assert(m_minPos <= position && position <= m_maxPos);
            const auto n = m_numElements;
            auto elements = m_elements.data();
            int pos = position - m_minPos + 1; // Make 1-relative.
            while(pos <= n)
            {
                elements[pos] += toAdd;
                assert(elements[pos] >= 0);
                pos += (pos & (pos * -1));
            }
        }
    private:
        int m_minPos;
        int m_maxPos;
        int m_numElements;
        vector<int64_t> m_elements;
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

    int64_t dbgQueryAnswer = -1;
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

void processTree(Node* node, SegmentTree& segmentTree)
{
    // Add the timeDepthDiff & amount added for the AddEvents associated with this node.
    for (const auto& addEvent : node->addEvents)
    {
        const int timeDepthDiff = node->depth - addEvent.time;
        segmentTree.addValueAt(timeDepthDiff, addEvent.numBacteriaToAdd);
    }
    // Process the "?" queries for this node.
    for (const auto& queryEvent : node->queryEvents)
    {
        const int timeDepthDiff = node->depth - queryEvent.time;
        const bool isLeaf = node->children.empty();
        queryEvent.originalQuery->queryAnswer = 0;
        if (isLeaf)
        {
            queryEvent.originalQuery->queryAnswer = segmentTree.numInRange(timeDepthDiff, segmentTree.maxPos());
        }
        else
        {
            queryEvent.originalQuery->queryAnswer = segmentTree.numInRange(timeDepthDiff, timeDepthDiff);
        }

    }

    for (auto child : node->children)
    {
        processTree(child, segmentTree);
    }

    // Pop the AddEvents we added when we first encountered this node.
    for (const auto& addEvent : node->addEvents)
    {
        const int timeDepthDiff = node->depth - addEvent.time;
        segmentTree.addValueAt(timeDepthDiff, -addEvent.numBacteriaToAdd);
    }
}

vector<int64_t> processQueries(vector<Node>& nodes, vector<Query>& queries)
{
    // Treat all bacteria originally in a node as an add event occurring at time -1
    // (simplifies the code).
    for (auto& node : nodes)
    {
        node.addEvents.push_back({nullptr, -1, node.initialBacteria});
    }

    // Record the AddEvents ('+') and QueryEvents ('?') associated with a node on that node.
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

    SegmentTree segmentTree(-queries.size(), +queries.size());
    processTree(&(nodes.front()), segmentTree);

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
        //const int N = rand() % 100 + 1;
        const int N = 500'000;
        //const int Q = rand() % 1000 + 1;
        //const int Q = N * 10;
        const int Q = 500'000;

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

    const int N = readInt();
    const int Q = readInt();

    vector<Node> nodes(N);

    for (int i = 0; i < N - 1; i++)
    {
        const int x = readInt() - 1;
        const int y = readInt() - 1;

        nodes[x].neighbours.push_back(&(nodes[y]));
        nodes[y].neighbours.push_back(&(nodes[x]));
    }

    for (int i = 0; i < N; i++)
    {
        nodes[i].nodeId = i + 1;
        nodes[i].initialBacteria = readInt();
    }

    vector<Query> queries(Q);

    for (int i = 0; i < Q; i++)
    {
        char queryType;
        scan_char(queryType);
        const int nodeId = readInt();

        queries[i].nodeId = nodeId;

        if (queryType == '+')
        {
            queries[i].isAddBacteria = true;
            queries[i].numBacteriaToAdd = readInt();
        }
    }

    fixParentAndChild(&(nodes.front()), 0, nullptr);

    const auto queryResults = processQueries(nodes, queries);
    for (const auto x : queryResults)
    {
        cout << x << endl;
    }

    assert(cin);
}
