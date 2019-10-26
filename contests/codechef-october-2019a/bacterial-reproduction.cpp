// Simon St James (ssjgz) - 2019-10-06
// 
// Solution to: https://www.codechef.com/OCT19A/problems/BACREP
//
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
// "Positions" are "TDD"'s - "Time-Depth Differences".
class SegmentTree
{
    public:
        SegmentTree() = default;
        SegmentTree(int minTDD, int maxTDD)
            : m_minTDD{minTDD},
              m_maxTDD{maxTDD},
              m_directValues(maxTDD - minTDD + 1),
              m_numElements{2 * (maxTDD - minTDD + 1)},
              m_elements(m_numElements + 1)
        {
        }
        // Find the number in the given range (inclusive) in O(log2(maxTDD - minTDD)).
        int64_t numBacteriaInTDDRange(int startTDD, int endTDD) const
        {
            startTDD -= m_minTDD;
            endTDD -= m_minTDD;
            startTDD++; // Make 1-relative.  startTDD and endTDD are inclusive.
            endTDD++;
            int64_t sum = 0;
            auto elements = m_elements.data();
            while(endTDD > 0)
            {
                sum += elements[endTDD];
                endTDD -= (endTDD & (endTDD*-1));
            }
            startTDD--;
            while(startTDD > 0)
            {
                sum -= elements[startTDD];
                startTDD -= (startTDD & (startTDD*-1));
            }
            return sum;
        }
        // Find the number in the range [tdd, maxTDD].
        int64_t numBacteriaWithTDDAtLeast(int tdd)
        {
            return numBacteriaInTDDRange(tdd, m_maxTDD);
        }

        void addBacteriaAtTDD(int tdd, int64_t toAdd)
        {
            assert(m_minTDD <= tdd && tdd <= m_maxTDD);
            tdd -= m_minTDD;
            m_directValues[tdd] += toAdd;
            const auto n = m_numElements;
            auto elements = m_elements.data();
            int pos = tdd + 1; // Make 1-relative.
            while(pos <= n)
            {
                elements[pos] += toAdd;
                assert(elements[pos] >= 0);
                pos += (pos & (pos * -1));
            }
        }
        int64_t numBacteriaAtTDD(int tdd)
        {
            assert(m_minTDD <= tdd && tdd <= m_maxTDD);
            return m_directValues[tdd - m_minTDD];
        }
    private:
        int m_minTDD;
        int m_maxTDD;
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
    int time = -1;
    Query* originalQuery = nullptr;
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

void processTree(Node* node, SegmentTree& ancestorAddEventTDDs)
{
    // Add the timeDepthDiff & amount added for the AddBacteriaEvents associated with this node to ancestorAddEventTDDs, for
    // use by our descendents.
    // NB: we are treating node as its own "ancestor" (and its own "descendent"!) for simplicity :)
    for (const auto& addBacteriaEvent : node->addBacteriaEvents)
    {
        const int timeDepthDiff = node->depth - addBacteriaEvent.time;
        ancestorAddEventTDDs.addBacteriaAtTDD(timeDepthDiff, addBacteriaEvent.numBacteriaToAdd);
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
            countBacteriaEvent.originalQuery->countBacteriaAnswer = ancestorAddEventTDDs.numBacteriaWithTDDAtLeast(timeDepthDiff);
        }
        else
        {
            countBacteriaEvent.originalQuery->countBacteriaAnswer = ancestorAddEventTDDs.numBacteriaAtTDD(timeDepthDiff);
        }

    }

    // Recurse.
    for (auto child : node->children)
    {
        processTree(child, ancestorAddEventTDDs);
    }

    // Leaving this node: "pop" the AddBacteriaEvents we added when we first encountered this node.
    for (const auto& addBacteriaEvent : node->addBacteriaEvents)
    {
        const int timeDepthDiff = node->depth - addBacteriaEvent.time;
        ancestorAddEventTDDs.addBacteriaAtTDD(timeDepthDiff, -addBacteriaEvent.numBacteriaToAdd);
    }
}

vector<int64_t> processQueries(vector<Node>& nodes, vector<Query>& queries)
{
    // Treat all bacteria originally in a node as an add event occurring at time -1
    // (simplifies the code).  This will lead to up to O(N) extra additions to the 
    // SegmentTree.  We could easily avoid these extra additions at the cost of special-casing
    // the initialBacteria in a couple of places in the code, but I'd rather keep things simple :)
    for (auto& node : nodes)
    {
        node.addBacteriaEvents.push_back({-1, node.initialBacteria});
    }

    // Record the AddEvents ('+') and CountBacteriaEvents ('?') associated with a node, on that node.
    for (int time = 0; time < queries.size(); time++)
    {
        auto& query = queries[time];
        auto& node = nodes[query.nodeId - 1];
        if (query.queryType == QueryType::AddBacteria)
        {
            node.addBacteriaEvents.push_back({time, query.numBacteriaToAdd});
        }
        else
        {
            node.countBacteriaEvents.push_back({time, &query});
        }
    }

    SegmentTree ancestorAddEventTDDs(-queries.size(),  // Minimum timeDepthDiff occurs when depth is 0 and we are dealing with the final query.
                                     +nodes.size() + 1); // Max timeDepthDiff occurs when depth is N and we are dealing with the initialBacteria query (t == -1).
    processTree(&(nodes.front()), ancestorAddEventTDDs);

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
    //
    // LONGER EXPLANATION
    //
    // Firstly, as mentioned in the so-called Quick Explanation, we classify each query as either a "Add Event" ("+", AddBacteriaEvent)
    // or a "Count Event" ("?", CountBacteriaEvent).  Each event has a *time* at which it occurs (derived from its position in
    // the list of queries) and a *node* which it affects.  We'll ignore the bacteria initially present in each node (initialBacteria)
    // until right at the end as they'd just complicate matters, otherwise.
    //
    // Secondly, we can track the contributions of a given Add Event ae which adds ae.numBacteriaToAdd bacteria to node ae.node at time 
    // ae.t independently from all the other globs of bacteria, whether said globs were there at the start of the simulation or
    // were added by an add event.  To see this, observe the following figure showing the evolution of the system from the beginning
    // of second ae.t to the end of second ae.t + 1; the number in each node denotes the number of bacteria in that node.
    //
    //
    //        (A)
    //         |
    //      v (B)          Beginning of time ae.t.
    //       / | \
    //    (C1)(C2)(C3)
    //
    //        (Z)
    //         |
    //      v (A)          Some short time through time ae.t, after reproduction step occurs.
    //       / | \
    //    (B) (B)(B)
    //
    //        (Z)
    //         |
    //     v (A+X)         Some short time later still through time ae.t, after we add X = ae.numBacteriaToAdd to
    //       / | \         node v = ae.node.
    //     (B)(B)(B)
    //
    //        (Y)
    //         |
    //      v (Z)           
    //       / | \         End of time ae.t + 1, after a further reproduction step.
    //  (A+X)(A+X)(A+X)
    //
    // We see that even though the reproduction rules consider only the total bacteria for a node, we can still "separate out" the
    // contributions of the X = ae.numBacteriaToAdd to the nodes at the simulation progresses.  We see that bacteria at a node v will
    // "drop" one level into the child nodes (i.e. increase their depth by 1) after each second (unless v is a leaf, in which case 
    // bacteria "pool" there - if a bacterium ends up in a leaf node, it's there forever).  In fact, it's hopefully easy to see that:
    //
    //    At the end of time t (t >= ae.t), the Add Event ae contributes ae.numBacteriaToAdd to the bacteria count for precisely the
    //    set of nodes v such that either:
    //
    //       i) v is a non-leaf and v is a descendent of ae.node and depth(ae.node) + (t - ae.t) == depth(v); or
    //       ii) v is a leaf and v is a descendent of ae.node and depth(ae.node) + (t - ae.t) >= depth(v).
    //
    // Now, the point of the question is to answer the "Count Event" queries ce i.e. counting the bacteria at node ce.node at the
    // end of the second ce.t.  Inverting the rule about contributions from Add Events ae above, we see that 
    //
    //    The number of bacteria in node ce.node at the end of the second ce.t is the sum of ae.numBacteriaToAdd over all Add Events ae
    // such that:
    //
    //      i) ae.node is an ancestor of ce.node; and
    //      ii) depth(ae.node) + ce.t - ae.t == depth(ce.node) (">=" if ce.node is a leaf node); and
    //      iii) ce.t >= ae.t
    //
    // We can re-write ii) as:
    //
    //      ii) ae.t - depth(ae.node) == ce.t - depth(ce.node) (again, ">=" if ce.node is a leaf node)
    //
    // From this, we see that the concept of the difference between the time of a an event and the depth of the node it affects seems
    // to be very important, so we give it a name - let the Time-Depth Difference (TDD) of an event e be TDD(e) = e.t - depth(e.node).  We may also write ii) as
    //
    //      ii) TDD(ae) == TDD(ce)
    //
    // if we want.
    //
    // As mentioned in the Quick Explanation, we are ultimately trying to re-order the queries/ events so that they are processed in the
    // order of visitation of the nodes they affect in a DFS from the root of the tree, but we must not break the "chronology" of the
    // events, as represented by condition iii).  What can we do?
    //
    // Well, it turns out that, if i) and ii) are satisfied, then so is iii), automagically! To see this, let ae be an Add Event and ce a
    // Count Event that satisfy i) and ii) i.e.
    //
    //     i) ae.node is an ancestor of ce.node; and
    //    ii) ae.t - depth(ae.node) == ce.t - depth(ce.node) [I'm going to assume ce.node is a non-leaf for simplicity; the logic is much the same for the leaf case].
    //   
    // Now, since ae.node is an ancestor of ce.node, we must have that depth(ae.node) <= depth(ce.node); thus:
    //
    //    ae.t - depth(ae.node) == ce.t - depth(ce.node)  
    // => ae.t - ce.t == depth(ae.node) - depth(ce.node)
    // => ae.t - ce.t <= 0
    // => ae.t <= ce.t
    // => ce.t >= ae.t
    //
    // i.e. condition iii) always holds, so we can ignore it!
    //
    // So, we can now process the queries/ Events in a different order, as follows:
    //
    // Do a DFS from the root of the tree (processTree).  When we enter a node u, find all Add Events ae with ae.node == u, and add ae.numBacteriaToAdd
    // to a segment tree (ancestorAddEventTDDs) at "position" TDD(ae) (= ae.time - depth(u)).  When we exit node u (after exploring all nodes 
    // that have u as an ancestor), undo these additions to the segment tree we made when we entered u.
    //
    // Now, say during our DFS we enter some node v which has a Count Event ce affecting it (i.e. ce.node == v).  What can we find at "position" TDD(ce) in the
    // segment tree? By the rules above, we'll find precisely the sum of ae.numBacteriaToAdd where ae.node is an ancestor of v and TDD(ae) == TDD(ce) 
    // i.e. exactly the answer to the query ce if v is a non-leaf!
    //
    // If v is a leaf, we instead just query the segment tree for the sum of entries at or to the right of "position" TDD(ce).
    //
    // And that's it! We still have to deal with the initial bacteria located in the nodes at the start of the simulation, but this is easy: 
    // it's hopefully clear that the simulation would proceed identically if we treated these as arising from some synthetic Add Events which add the
    // requisite bacteria to each node en masse 1 second before the simulation begins i.e. at t == -1.  We add these synthetic Add Events in
    // the first step of processQueries.
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
