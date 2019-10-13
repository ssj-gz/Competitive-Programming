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
              m_directValues(maxPos - minPos + 1),
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
            position -= m_minPos;
            m_directValues[position] += toAdd;
            const auto n = m_numElements;
            auto elements = m_elements.data();
            int pos = position + 1; // Make 1-relative.
            while(pos <= n)
            {
                elements[pos] += toAdd;
                assert(elements[pos] >= 0);
                pos += (pos & (pos * -1));
            }
        }
        int64_t numAt(int position)
        {
            assert(m_minPos <= position && position <= m_maxPos);
            return m_directValues[position - m_minPos];
        }
    private:
        int m_minPos;
        int m_maxPos;
        vector<int64_t> m_directValues;
        int m_numElements;
        vector<int64_t> m_elements;
};

struct Query;
enum class QueryType { Unknown, AddBacteria, CountBacteria };
struct AddBacteriaEvent
{
    int time = -1;
    int64_t numBacteriaToAdd = -1;
};

struct CountBacteriaEvent
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

    vector<AddBacteriaEvent> addBacteriaEvents;
    vector<CountBacteriaEvent> countBacteriaEvents;
};

struct Query
{
    QueryType queryType = QueryType::Unknown;
    int nodeId = -1;

    int64_t numBacteriaToAdd = -1;     // Only used when queryType is AddBacteria.
    int64_t countBacteriaAnswer = -1;  // Only used when queryType is CountBacteria.
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
    // Add the timeDepthDiff & amount added for the AddBacteriaEvents associated with this node, for
    // use by our descendents.
    // NB: we are treating node as its own "ancestor" (and its own "descendent"!) for simplicity :)
    for (const auto& addBacteriaEvent : node->addBacteriaEvents)
    {
        const int timeDepthDiff = node->depth - addBacteriaEvent.time;
        ancestorAddEventTimeDepthDiffs.addValueAt(timeDepthDiff, addBacteriaEvent.numBacteriaToAdd);
    }
    // Process the CountBacteria queries for this node.
    for (const auto& countBacteriaEvent : node->countBacteriaEvents)
    {
        // An Add Event of x bacteria at an ancestor node ancestorNode at time t and depth d will
        // end up at exactly node at time countBacteriaEvent.time if and only if d - t == node->depth - countBacteriaEvent.time.
        // Such an add event will end up at exactly node x *or* overshoot x at time countBacteriaEvent.time if and only if
        // d - t >= node->depth - countBacteriaEvent.time (which is what we want for leaf nodes).
        const int timeDepthDiff = node->depth - countBacteriaEvent.time;
        const bool isLeaf = node->children.empty();
        countBacteriaEvent.originalQuery->countBacteriaAnswer = 0;
        if (isLeaf)
        {
            countBacteriaEvent.originalQuery->countBacteriaAnswer = ancestorAddEventTimeDepthDiffs.numAtOrToRightOf(timeDepthDiff);
        }
        else
        {
            countBacteriaEvent.originalQuery->countBacteriaAnswer = ancestorAddEventTimeDepthDiffs.numAt(timeDepthDiff);
        }

    }

    for (auto child : node->children)
    {
        processTree(child, ancestorAddEventTimeDepthDiffs);
    }

    // Pop the AddBacteriaEvents we added when we first encountered this node.
    for (const auto& addBacteriaEvent : node->addBacteriaEvents)
    {
        const int timeDepthDiff = node->depth - addBacteriaEvent.time;
        ancestorAddEventTimeDepthDiffs.addValueAt(timeDepthDiff, -addBacteriaEvent.numBacteriaToAdd);
    }
}

vector<int64_t> processQueries(vector<Node>& nodes, vector<Query>& queries)
{
    // Treat all bacteria originally in a node as an add event occurring at time -1
    // (simplifies the code).  This will lead to up to O(N) extra additions to the 
    // SegmentTree.  We can easily avoid these extra additions at the cost of special-casing
    // the initialBacteria in a couple of places in the code.
    for (auto& node : nodes)
    {
        node.addBacteriaEvents.push_back({-1, node.initialBacteria});
    }

    // Record the AddEvents ('+') and CountBacteriaEvents ('?') associated with a node, on that node.
    for (int time = 0; time < queries.size(); time++)
    {
        auto node = &(nodes[queries[time].nodeId - 1]);
        if (queries[time].queryType == QueryType::AddBacteria)
        {
            node->addBacteriaEvents.push_back({time, queries[time].numBacteriaToAdd});
        }
        else
        {
            node->countBacteriaEvents.push_back({&(queries[time]), time});
        }
    }

    SegmentTree ancestorAddEventTimeDepthDiffs(-queries.size(), +queries.size());
    processTree(&(nodes.front()), ancestorAddEventTimeDepthDiffs);

    // Collect the actual results of the "?" queries.
    vector<int64_t> countBacteriaQueryResults;
    for (const auto& query : queries)
    {
        if (query.queryType == QueryType::CountBacteria)
        {
            assert(query.countBacteriaAnswer != -1);
            countBacteriaQueryResults.push_back(query.countBacteriaAnswer);
        }
    }
    
    return countBacteriaQueryResults;
}


int main(int argc, char* argv[])
{
    // QUICK(-ish) EXPLANATION
    //
    // Divide the queries into "Add Events" and "Query Events", both affecting a given node at a given time, 
    // and treat the initial bacteria count at a node v as a special case of the former (i.e. an Add Event affecting 
    // node v at time t = -1 - this makes the code a little simpler).
    //
    // It can be shown that the Count of bacteria at (non-leaf) node v at a time t is simply the sum, over all ancestors u of
    // v, of all bacteria added in Add Events affecting node u at time t' satisfying 
    //
    //     t' - depth(u) == t - depth(v)
    //
    // (with the "==" change to ">=" in the case where v is a leaf).
    //
    // Thus, by doing a DFS from the root and, when encountering a node u, adding, for each t' such that an Add Event adds X
    // bacteria to u, X at the position t' - depth(u) to a Segment Tree, by the time we get to node v we can deduce precisely
    // the Count of bacteria at node v at any time t just by querying the Segment Tree for the number of bacteria at position
    // (or at position greater than or equal to, if v is a leaf) t - depth(v).
    //
    // That is, we process the Add/ Count events not in the original order in which the appear in the list of queries but in 
    // the order in which the node they affect appears in our DFS.
    //
    // In many problems, interfering with the order of the Events might upset the Chronology: i.e. we might (incorrectly) Count
    // bacteria at time t that won't even be Added until a time *after* t! - but in this particular Problem it can be shown
    // that this cannot happen, so processing the queries in a different order does not affect the result.
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
            queries[i].queryType = QueryType::AddBacteria;
            queries[i].numBacteriaToAdd = readInt();
        }
        else
        {
            queries[i].queryType = QueryType::CountBacteria;
        }
    }

    // Process the queries ...
    fixParentAndChild(&(nodes.front()), 0, nullptr);
    const auto countBacteriaQueryResults = processQueries(nodes, queries);

    // ... and print the results!
    for (const auto countBacteriaQueryResult : countBacteriaQueryResults)
    {
        cout << countBacteriaQueryResult << endl;
    }

    assert(cin);
}
