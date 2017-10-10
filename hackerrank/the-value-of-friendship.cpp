// Simon St James (ssjgz) - 2017-10-10 10:04
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

struct Edge;
struct Node
{
    vector<Edge*> neighbours;
    bool isInComponent = false;
};
struct Edge
{
    Node* node1 = nullptr;
    Node* node2 = nullptr;
    Node* otherNode(Node* node)
    {
        return (node == node1 ? node2 : node1);
    }
    bool isInComponent = false;
};


struct ComponentInfo
{
    int numNodes = 0;
    int numEdges = 0;
};

vector<ComponentInfo> buildComponentInfoInDecreasingSize(vector<Node>& nodes)
{
    vector<ComponentInfo> componentsInfo;
    for (auto& node : nodes)
    {
        if (node.isInComponent)
            continue;

        ComponentInfo componentInfo;
        vector<Node*> nodesToExplore = { &node };

        while (!nodesToExplore.empty())
        {
            vector<Node*> nextNodesToExplore;
            for (auto& nodeToExplore : nodesToExplore)
            {
                if (nodeToExplore->isInComponent)
                    continue;

                nodeToExplore->isInComponent = true;
                componentInfo.numNodes++;

                for (auto edge : nodeToExplore->neighbours)
                {
                    if (!edge->isInComponent)
                    {
                        edge->isInComponent = true;
                        componentInfo.numEdges++;
                    }
                    nextNodesToExplore.push_back(edge->otherNode(nodeToExplore));
                }

            }

            nodesToExplore = nextNodesToExplore;
        }

        componentsInfo.push_back(componentInfo);
    }
    sort(componentsInfo.begin(), componentsInfo.end(), [](const auto& lhs, const auto& rhs)
            {
                return rhs.numNodes < lhs.numNodes;
            });
    return componentsInfo;
}

int main()
{
    // A fairly easy one to solve, but an absolute pig to prove: I usually like to
    // give at least a sketch-proof of why something works, but this one was proving
    // so laborious that I just gave up XD
    //
    // So: it's fundamentally a Graph Theory problem: the students are nodes, and an explicit
    // friendship between students in an edge between them; then, after adding an edge,
    // the value we add at this step is, for each student, the number of students in
    // the component containing that student.
    //
    // Let {e1, e2, ... , em } be a sequence of edges (direct friendships added); then
    // the final graph can be expressed as a set of strongly-connected components C1, C2, ..., Ck
    // for some k.  If an edge ei connects two nodes that are already in the same component (i.e. after
    // adding e1, ... ei-1), then call it a "reinforcing" edge; else call it a "growing" edge.  For a connected
    // component C, let SP(C) be some spanning tree of C; if C has n nodes, then SP(C) has n - 1 edges.
    // It's reasonably easy to see that all edges that are not part of SP(C) are reinforcing edges.
    //
    // I claim the following, without proof:
    //
    // - The order of edges that gives the largest value is one that first fills in the subgraph
    //   SP(C1) + SP(C2) + ... + SP(Ck) by adding the growing edges, then adds the remaining reinforcing edges
    //   (not too hard to prove; take any "optimal" arrangement where a reinforcing edge is added before a growing edge;
    //   pick the last such, and swap the order of the reinforcing and growing edge; the total value will be at least
    //   as high).
    // - The optimal order for filling in a component SP(Ci) is to pick any edge in SP(Ci) and then add subsequent edges
    //   that share a node with an edge already added i.e. we grow Ci as one, solid component, and never "split" it
    //   into a forest.  Call this strategy "growing connectedly". 
    //   This is harder to prove, but as a sketch, imagine that we did split SP(Ci) as a forest:
    //   then at some point, because we ultimately connect it up, we must add an edge that connects two previously
    //   disjoint subtrees, T1 and T2, say. Let e, T1 and T2 be the first of these, and assume T1 is at least as large as T2, 
    //   and let v be the vertex in T2 that e joins to.  T1 and T2 must have been grown connectedly (as e is the first 
    //   edge that connects two trees) so we can re-order the edges used to create T2 so that the edge that adds v to T2
    //   is not last (it can be shown that any re-ordering of edges that still grows things connectedly results in the same value).
    //   Let e' be the last edge added in T2 prior to e: then it can be shown that swapping the order of e and e'
    //   in our solution leads to a solution with at least as high a value, but where the size of the smaller of the two
    //   first trees to become joined is smaller: by repeating this, we can gradually shrink T2 down to nothing,
    //   so that T1+T2 was grown in a connected fashion.  If there is are still at T'1 and T'2 that are the first
    //   trees to be joined in our solution, we can repeat until one of these is eliminated; and so on.
    //   Eventually, we transform our original "optimal" solution into one of at least equal value that was grown
    //   in connectedly.
    // - If Ci has l nodes, then the optimal order for filling in SP(Ci) (growing it "connectedly", as described above)
    //   adds, ignoring any other components for the time being, 2 * (2 - 1) + 3 * (3 - 1) + ... + l * (l - 1) value
    //   (easy to see).
    // - The optimal order for filling in SP(C1) + SP(C2) + ... + SP(Ck) is to pick the Ci with the largest number of
    //   nodes and build that connectedly; then the component with the next highest number of nodes; etc (an absolute
    //   pig to prove!)
    //
    // So in a nutshell: find the components C1, C2 ... Ck by adding all edges; sort them in order of decreasing number
    // of nodes; build the spanning trees for each of the ordered Ci connectedly; then add the contribution of the 
    // reinforcing nodes i.e. those not used in any of the spanning tree.  We maintain the "value to add" for 
    // each already-built component in valuesOfProcessedComponents.
    int Q;
    cin >> Q;

    for (int q = 0; q < Q; q++)
    {
        int n, m;
        cin >> n >> m;

        vector<Node> nodes(n);
        vector<Edge> edges;
        edges.reserve(m);

        for (int i = 0; i < m; i++)
        {
            int x, y;
            cin >> x >> y;
            x--;
            y--;

            auto node1 = &(nodes[x]);
            auto node2 = &(nodes[y]);
            Edge edge;
            edge.node1 = node1;
            edge.node2 = node2;
            edges.push_back(edge);

            auto newEdge = &(edges.back());

            node1->neighbours.push_back(newEdge);
            node2->neighbours.push_back(newEdge);
        }

        const vector<ComponentInfo> componentInfoDecreasingNodes = buildComponentInfoInDecreasingSize(nodes);


        // Fill in each components Spanning Tree connectedly, starting with the largest.
        int64_t value = 0;
        int64_t valuesOfProcessedComponents = 0;
        int64_t numReinforcingEdges = 0;
        for (const auto& componentInfo : componentInfoDecreasingNodes)
        {
            const int numEdgesInComponentMST = componentInfo.numNodes - 1;
            numReinforcingEdges += (componentInfo.numEdges - numEdgesInComponentMST);

            for (int64_t i = 1; i <= numEdgesInComponentMST; i++)
            {
                value += valuesOfProcessedComponents + (i + 1) * i;
            }
            const int64_t valueOfThisComponent = (numEdgesInComponentMST + 1) * numEdgesInComponentMST;
            valuesOfProcessedComponents += valueOfThisComponent;
        }
        // Add contribution of remaining reinforcing edges.
        value += numReinforcingEdges * valuesOfProcessedComponents;

        cout << value << endl;
    }

}
