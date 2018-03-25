#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <cassert>
#include <sys/time.h>

// TODO - 
//
// - Read in testcase; form graph with Nodes and Edges.
// - Generate random testcases.
// - Crudely splice in the centroid decomposition stuff from Ticket to Ride.
// - Maybe fix up the decomposition stuff so that edges end up being actually removed - need to keep a "backup" of original edges, if so.  In any case, this can be left unti later.
// - Crudely splice in SuffixTreeBuilder.
// - Modify SuffixTreeBuilder so we can find the list of words that the currently read word is a suffix of.
// - Do the main processCentroid algorithm, and hook it into the decomposition stuff.
//   - Track word cursor as we do depth-first search; find longest word; transmit back along path from centroid; track the word suffixes we've generated; etc.
// - Do the final calculation of max crossed words for each node, etc.

using namespace std;

struct Edge;
struct Node
{
    vector<Edge*> neighbours;
};
struct BestTracker
{
    static constexpr int maxToStore = 3;
    void add(int64_t value, Edge* otherEdge);
    int num = 0;
    struct Blah
    {
        int64_t value = -1;
        Edge* otherEdge = nullptr;
    };
    Blah stored[maxToStore];
};
struct Edge
{
    Node* nodeA = nullptr;
    Node* nodeB = nullptr;
    int edgeId = -1;
    BestTracker bestTracker;
};
void BestTracker::add(int64_t value, Edge* otherEdge)
{
    //cout << "add: " << value << " otherEdge: " << otherEdge->edgeId << endl;
    assert(num <= maxToStore);
    for (int i = 0; i < num; i++)
    {
        if (stored[i].otherEdge == otherEdge)
        {
            stored[i].value = max(stored[i].value, value);
            return;
        }
    }
    vector<Blah> blee(begin(stored), begin(stored) + num);
    Blah newBlah;
    newBlah.value = value;
    newBlah.otherEdge = otherEdge;
    blee.push_back(newBlah);
    sort(blee.begin(), blee.end(), [](const auto& lhs,  const auto& rhs) { return lhs.value > rhs.value; });
    const int newNum = min(static_cast<int>(blee.size()), maxToStore);
    copy(blee.begin(), blee.begin() + newNum, stored);
    num = newNum;
};
struct PathValue
{
    PathValue() = default;
    PathValue(Edge* edgeA, Edge* edgeB, int64_t value)
        : edgeA{edgeA}, edgeB{edgeB}, value{value}
    {
    }
    Edge* edgeA = nullptr;
    Edge* edgeB = nullptr;
    int64_t value = 0;
};


ostream& operator<<(ostream& os, const PathValue& pathValue)
{
    os << "pathValue - value: " << pathValue.value << " edgeA: " << (pathValue.edgeA ? pathValue.edgeA->edgeId : -1) << " edgeB: " << (pathValue.edgeB ? pathValue.edgeB->edgeId : -1);
    return os;
}

int main()
{
    struct timeval time;
    gettimeofday(&time,NULL);
    srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
    
    const int maxNumNeighbours = 20;
    const int maxMaxValue = 100;
    const int maxPathValues = 20;

    int numTests = 0;

    while (true)
    {
        Node rootNode;
        const int numNeighbours = (rand() % (maxNumNeighbours - 1)) + 2;
        vector<Edge> edges(numNeighbours);
        int edgeId = 0;
        for (auto& edge : edges)
        {
            edge.nodeA = &rootNode;
            edge.edgeId = edgeId;
            edgeId++;
        }

        const int numPathValues = (rand() % maxPathValues) + 1;
        const int64_t maxValue = (rand() % maxMaxValue) + 1;
        vector<PathValue> pathValues;
        for (int i = 0; i < numPathValues; i++)
        {
            while (true)
            {
                auto edge1 = &(edges[rand() % numNeighbours]);
                auto edge2 = &(edges[rand() % numNeighbours]);
                if (edge1 == edge2)
                    continue;


                pathValues.push_back({edge1, edge2, (rand() % maxValue) + 1});
                break;
            }
        }

        for (const auto& pathValue : pathValues)
        {
            //cout << pathValue << endl;
        }
        for (const auto& pathValue : pathValues)
        {
            pathValue.edgeA->bestTracker.add(pathValue.value, pathValue.edgeB); 
            pathValue.edgeB->bestTracker.add(pathValue.value, pathValue.edgeA); 
        }

        auto shareAnEdge = [](const PathValue& pathValue1, const PathValue& pathValue2)
        {
            return !( pathValue1.edgeA != pathValue2.edgeA && pathValue1.edgeA != pathValue2.edgeB &&
                    pathValue1.edgeB != pathValue2.edgeA && pathValue1.edgeB != pathValue2.edgeB);
        };

        int64_t dbgMaxPathValueProduct = -1;
        for (const auto& pathValue1 : pathValues)
        {
            for (const auto& pathValue2 : pathValues)
            {
                if (!shareAnEdge(pathValue1, pathValue2))
                {
                    const auto pathValueProduct = pathValue1.value * pathValue2.value;
                    if (pathValueProduct > dbgMaxPathValueProduct)
                    {
                        dbgMaxPathValueProduct = pathValueProduct;
                        //cout << "New best: " << pathValueProduct << " from " << pathValue1 << " and " << pathValue2 << endl;
                    }
                }
            }
        }

        //cout << "dbgMaxPathValueProduct: " << dbgMaxPathValueProduct << endl;

        auto comparePathValues = [](const PathValue& lhs, const PathValue& rhs) 
        {
            if (lhs.value != rhs.value)
                return lhs.value > rhs.value;
            if (lhs.edgeA != rhs.edgeA)
                return lhs.edgeA < rhs.edgeA;
            return lhs.edgeB < rhs.edgeB;
        };
        set<PathValue, decltype(comparePathValues)> pathValuesByVal(comparePathValues);

        int64_t maxPathValueProduct = -1;
        for (auto& edge : edges)
        {
            int64_t maxFromThis = -1;
            for (auto blahIter = begin(edge.bestTracker.stored); blahIter != begin(edge.bestTracker.stored) + edge.bestTracker.num; blahIter++)
            {
                PathValue blee(&edge, blahIter->otherEdge, blahIter->value);
                //cout << " Blee: " << blee << endl;
                for (const auto& otherPathValue : pathValuesByVal)
                {
                    //cout << " otherPathValue: " << otherPathValue << endl;
                    if (!shareAnEdge(blee, otherPathValue))
                    {
                        maxFromThis = max(maxFromThis, blee.value * otherPathValue.value);
                        break;
                    }
                }
                maxPathValueProduct = max(maxPathValueProduct, maxFromThis);
            }
            for (auto blahIter = begin(edge.bestTracker.stored); blahIter != begin(edge.bestTracker.stored) + edge.bestTracker.num; blahIter++)
            {
                PathValue blee(&edge, blahIter->otherEdge, blahIter->value);
                pathValuesByVal.insert(blee);
            }
        }
        cout << "maxPathValueProduct: " << maxPathValueProduct << " dbgMaxPathValueProduct: " << dbgMaxPathValueProduct << " " << (maxPathValueProduct == dbgMaxPathValueProduct ? "MATCH" : "NOMATCH") <<  " numTests: " << numTests << endl;
        assert(maxPathValueProduct == dbgMaxPathValueProduct);
        numTests++;
    }

}
