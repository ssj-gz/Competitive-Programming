// Simon St James (ssjgz) - 2019-10-06
// 
// Solution to: https://www.codechef.com/OCT19A/problems/BACREP
//
#define SUBMISSION
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>

#include <cassert>

using namespace std;

// Lots of input to read, so use ultra-fast reader.
void scan_integer( int &x )
{
    int c = getchar_unlocked();
    x = 0;
    for( ; ((c<48 || c>57)); c = getchar_unlocked() );
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

char readChar()
{
    char toRead;
    scan_char(toRead);
    return toRead;
}

int readInt()
{
    int toRead;
    scan_integer(toRead);
    assert(cin);
    return toRead;
}

// Typical SegmentTree - you can find similar implementations all over the place :)
// This one has been modified to accept negative indices/ positions.
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
        // Find the number in the given range (inclusive) in O(log2(maxId)).
        int64_t numInRange(int startPosition, int endPosition) const
        {
            startPosition -= m_minPos;
            endPosition -= m_minPos;
            startPosition++; // Make 1-relative.  startPosition and endPosition are inclusive.
            endPosition++;
            int64_t sum = 0;
            auto elements = m_elements.data();
            while(endPosition > 0)
            {
                sum += elements[endPosition];
                endPosition -= (endPosition & (endPosition*-1));
            }
            startPosition--;
            while(startPosition > 0)
            {
                sum -= elements[startPosition];
                startPosition -= (startPosition & (startPosition*-1));
            }
            return sum;
        }
        // Find the number in the range [position, maxPos].
        int64_t numAtOrToRightOf(int position)
        {
            return numInRange(position, m_maxPos);
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

void processTree(Node* node, SegmentTree& ancestorAddEventTimeDepthDiffs)
{
    // Add the timeDepthDiff & amount added for the AddEvents associated with this node.
    // NB: we are treating node as its own "ancestor" for simplicity :)
    for (const auto& addEvent : node->addEvents)
    {
        const int timeDepthDiff = node->depth - addEvent.time;
        ancestorAddEventTimeDepthDiffs.addValueAt(timeDepthDiff, addEvent.numBacteriaToAdd);
    }
    // Process the "?" queries for this node.
    for (const auto& queryEvent : node->queryEvents)
    {
        // An add event of x bacteria at an ancestor node ancestorNode at time t and depth d will
        // end up at exactly node if and only if d - t == node->depth - queryEvent.time.
        // Such an add event will end up at exactly node x or overshoot x if and only if
        // d - t >= node->depth - queryEvent.time (which is what we want for leaf nodes).
        const int timeDepthDiff = node->depth - queryEvent.time;
        const bool isLeaf = node->children.empty();
        queryEvent.originalQuery->queryAnswer = 0;
        if (isLeaf)
        {
            queryEvent.originalQuery->queryAnswer = ancestorAddEventTimeDepthDiffs.numAtOrToRightOf(timeDepthDiff);
        }
        else
        {
            queryEvent.originalQuery->queryAnswer = ancestorAddEventTimeDepthDiffs.numInRange(timeDepthDiff, timeDepthDiff);
        }

    }

    for (auto child : node->children)
    {
        processTree(child, ancestorAddEventTimeDepthDiffs);
    }

    // Pop the AddEvents we added when we first encountered this node.
    for (const auto& addEvent : node->addEvents)
    {
        const int timeDepthDiff = node->depth - addEvent.time;
        ancestorAddEventTimeDepthDiffs.addValueAt(timeDepthDiff, -addEvent.numBacteriaToAdd);
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

    SegmentTree ancestorAddEventTimeDepthDiffs(-queries.size(), +queries.size());
    processTree(&(nodes.front()), ancestorAddEventTimeDepthDiffs);

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

    const int N = readInt();
    const int Q = readInt();

    // Read and store graph.
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
        nodes[i].initialBacteria = readInt();
    }

    // Read and store queries.
    vector<Query> queries(Q);
    for (int i = 0; i < Q; i++)
    {
        const auto queryType = readChar();
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
