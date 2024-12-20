// Simon St James (ssjgz) - 2019-11-02
// 
// Solution to: https://www.codechef.com/NOV19A/problems/FAILURE
//
#include <iostream>
#include <vector>
#include <algorithm>

#include <cassert>

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

struct Node
{
    vector<Node*> neighbours;
    int id = -1;
    bool isInCycle = false;
    Node* nextInCycle = nullptr;
    bool isConsecutiveWithInCycle(Node* otherNode)
    {
        return isInCycle && otherNode->isInCycle && 
               (otherNode->nextInCycle == this || this->nextInCycle == otherNode);
    }

    bool visitedInDFS = false;
    bool isInDFSStack = false;
    bool isRemoved = false;
    int componentNum = -1;

    int numComponentCyclesBreaks = 0;
};

struct NodeAndOccurences
{
    Node* node = nullptr;
    int numOccurences = -1;
};

class NodeMultiSet
{
    public:
        NodeMultiSet(int maxNodeId)
            : m_numNodesWithId(maxNodeId + 1, 0)
        {
        }
        // O(1).
        void addNode(Node* nodeToAdd)
        {
            if (m_numNodesWithId[nodeToAdd->id] == 0)
            {
                m_nodesInSet.push_back(nodeToAdd);
            }
            m_numNodesWithId[nodeToAdd->id]++;
        }
        // O(number of distinct nodes in set).
        vector<NodeAndOccurences> nodesAndOccurrences() const
        {
            vector<NodeAndOccurences> nodesAndOccurrences;
            for (auto node : m_nodesInSet)
            {
                nodesAndOccurrences.push_back({node, m_numNodesWithId[node->id]});
            }
            return nodesAndOccurrences;
        }
        // O(number of distinct nodes in set) i.e. the cost of clearing the NodeMultiSet is 
        // amortised into the cost of populating it in the first place.
        void clear()
        {
            for (auto node : m_nodesInSet)
            {
                m_numNodesWithId[node->id] = 0;
            }
            m_nodesInSet.clear();
        }
    private:
        vector<Node*> m_nodesInSet;
        vector<int> m_numNodesWithId;
};

// Well-known algorithm for extracting a cycle, if one exists.
// Which one you get is more-or-less arbitrary.
void findACycleAux(Node* currentNode, Node* parentNode, vector<Node*>& destCycle, vector<Node*>& ancestors)
{
    if (currentNode->isRemoved)
        return;
    if (currentNode->visitedInDFS)
    {
        if (currentNode->isInDFSStack && destCycle.empty())
        {
            auto ancestorsCopy = ancestors;
            assert(!ancestorsCopy.empty());
            destCycle.push_back(currentNode);
            while (ancestorsCopy.back() != currentNode)
            {
                destCycle.push_back(ancestorsCopy.back());
                ancestorsCopy.pop_back();
            }
        }
        return;
    }
    currentNode->visitedInDFS = true;
    ancestors.push_back(currentNode);
    currentNode->isInDFSStack = true;

    for (auto childNode : currentNode->neighbours)
    {
        if (childNode == parentNode)
        {
            continue;
        }

        findACycleAux(childNode, currentNode, destCycle, ancestors);
    }

    ancestors.pop_back();
    currentNode->isInDFSStack = false;

}

// Returns some arbitrary cycle in the component containing startNode
// if and only if there is a cycle present in that component.
// (If not, returns an empty list).
vector<Node*> findACycle(Node* startNode, vector<Node>& nodes)
{
    vector<Node*> cycle;
    vector<Node*> ancestors;
    findACycleAux(startNode, nullptr, cycle, ancestors);
    
    return cycle;
}

// Helper for getComponents.
vector<Node*> getComponent(Node* rootNode, int markAsComponentNum)
{
    assert(rootNode->componentNum == -1);
    assert(!rootNode->isRemoved);
    vector<Node*> component = { rootNode };
    vector<Node*> toExplore = {rootNode};
    rootNode->componentNum = markAsComponentNum;

    while (!toExplore.empty())
    {
        vector<Node*> nextToExplore;
        for (auto node : toExplore)
        {
            for (auto neighbour : node->neighbours)
            {
                if (!neighbour->isRemoved && neighbour->componentNum == -1)
                {
                    neighbour->componentNum = markAsComponentNum;
                    component.push_back(neighbour);
                    nextToExplore.push_back(neighbour);
                }
            }
        }

        toExplore = nextToExplore;
    }

    return component;

}

// Decompose the graph into fully-connected components in O(N).
// Nodes marked as isRemoved are ignored.
vector<vector<Node*>> getComponents(vector<Node>& nodes)
{
    for (auto& node : nodes)
    {
        node.componentNum = -1;
    }
    vector<vector<Node*>> components;
    for (auto& startNode : nodes)
    {
        if (!startNode.isRemoved && startNode.componentNum == -1)
        {
            auto component = getComponent(&startNode, components.size());
            components.push_back(component);
        }
    }
    return components;
}

bool componentHasCycle(const vector<Node*>& component)
{
    int numEdgesTimesTwo = 0;
    for (const auto node : component)
    {
        assert(!node->isRemoved);
        for (auto neighbour : node->neighbours)
        {
            if (!neighbour->isRemoved)
                numEdgesTimesTwo++;
        }
    }
    assert(numEdgesTimesTwo % 2 == 0);
    const auto numEdges = numEdgesTimesTwo / 2;
    assert(numEdges >= component.size() - 1);
    // A connected component over K nodes is a tree if and only if it has precisely K - 1 edges.
    return numEdges != component.size() - 1;
}

void addSyntheticEdgesBetweenConnectedNonConsecutiveCycleNodes(vector<Node>& nodes)
{
    // Introduce synthetic nodes so that no two non-consecutive cycle
    // nodes are connected; e.g. in the example below, NA and NB are
    // directly connected by an edge even though they are not consecutive
    // in the cycle (the cycle-nodes C and B lie between NA and NB, for example).
    //
    // ┌─────...────NB───...───────────────────┐ ┐
    // C            │                          │ │
    // │            │                          │ │
    // .            │                          │ │
    // .            │                          │ │
    // .            │                          │ │
    // │            │                          │ ├ Cycle C
    // NA───────────┘                          │ │ 
    // │                                       │ │
    // └─────...────B───...────────────────────┘ ┘        
    //
    // We introduce a synthetic node s, leaving the graph looking like:
    //
    // ┌─────...────NB───...───────────────────┐ ┐
    // C            │                          │ │
    // │            │                          │ │
    // .            │                          │ │
    // .            │                          │ │
    // .            │                          │ │
    // │            │                          │ ├ Cycle C
    // NA───────────s                          │ │ 
    // │                                       │ │
    // └─────...────B───...────────────────────┘ ┘        
    vector<std::pair<Node*, Node*>> directEdgesToReplace;
    for (auto& node : nodes)
    {
        if (!node.isInCycle)
            continue;

        for (auto neighbour : node.neighbours)
        {
            if (!neighbour->isInCycle)
                continue;
            if (node.isConsecutiveWithInCycle(neighbour))
                continue;

            if (node.id < neighbour->id) // Ensure we don't process the same "pair" more than once.
            {
                directEdgesToReplace.push_back({&node, neighbour});
            }
        }

        node.neighbours.erase(remove_if(node.neighbours.begin(), node.neighbours.end(), 
                                        [&node](const auto neighbour) 
                                        { 
                                            return node.isConsecutiveWithInCycle(neighbour); 
                                        }),
                              node.neighbours.end());
    }
    // Having removed edges between pairs of non-consecutive cycle nodes, add the synthetic node
    // as a bridge between them.
    for (const auto edge : directEdgesToReplace)
    {
        assert(nodes.size() + 1 <= nodes.capacity());
        nodes.push_back(Node());
        auto newSyntheticNode = &(nodes.back());

        edge.first->neighbours.push_back(newSyntheticNode);
        newSyntheticNode->neighbours.push_back(edge.first);
        
        edge.second->neighbours.push_back(newSyntheticNode);
        newSyntheticNode->neighbours.push_back(edge.second);
    }

}

int findSinglePointOfFailure(vector<Node>& nodes)
{
    // Try and find a cycle in any of the components of the graph.
    vector<Node*> cycle;
    for (const auto& component : getComponents(nodes))
    {
        cycle = findACycle(component.front(), nodes);
        if (!cycle.empty())
            break;
    }

    if (cycle.empty())
    {
        // No cycles anywhere in the original graph -> no single point
        // of failure.
        return -1;
    }

    for (int i = 0; i < cycle.size(); i++)
    {
        cycle[i]->isInCycle = true;
        cycle[i]->isRemoved = true;
        cycle[i]->nextInCycle = cycle[(i + 1) % cycle.size()];
    }

    // Deal with cycles caused by edges between nodes in C which are
    // not consecutive in C, without special casing.
    addSyntheticEdgesBetweenConnectedNonConsecutiveCycleNodes(nodes);

    // Get the list of components remaining after we've removed C.
    const vector<vector<Node*>> componentsWithoutC = getComponents(nodes);

    for (const auto& component : componentsWithoutC)
    {
        if (componentHasCycle(component))
        {
            // A cycle exists that has no nodes in common with C;
            // no way of removing a single node and breaking both
            // C and this cycle.
            return -1;
        }
    }

    // None of the componentsWithoutC has a cycle in G-C.
    NodeMultiSet cycleNodesConnectedToComponent(nodes.size());
    int numComponentsNeedToBreak = 0;
    for (const auto& component : componentsWithoutC)
    {
        cycleNodesConnectedToComponent.clear();
        for (const auto& node : component)
        {
            for (auto neighbour : node->neighbours)
            {
                if (neighbour->isInCycle)
                {
                    cycleNodesConnectedToComponent.addNode(neighbour);
                }
            }
        }

        const auto cycleNodesConnectedToComponentOccurrences = cycleNodesConnectedToComponent.nodesAndOccurrences();
        if (cycleNodesConnectedToComponentOccurrences.empty() || (cycleNodesConnectedToComponentOccurrences.size() == 1 && cycleNodesConnectedToComponentOccurrences.front().numOccurences == 1))
        {
            // This component would have no cycles even when C is added back in.
            // (In the original graph C, this component would look like a tree sprouting out
            // of one of the nodes in the cycle C).
            continue;
        }

        // This component has a cycle in G but not in G-C - removal of the SPF would need to
        // break the cycle in this component.
        numComponentsNeedToBreak++;

        if (cycleNodesConnectedToComponentOccurrences.size() >= 3)
        {
            //          D──...───E──...───F        ┐
            //          │        │        │        ├ Component
            //          │        │        │        ┘
            // ┌──...──CA──...──CB──...──CC─...──┐ ┐
            // │                                 │ ├ Cycle C
            // └─────────────────────────────────┘ ┘        

            // 3 distinct cycle nodes connected (CA, CB, CC) to the component; the cycles cannot be broken 
            // by any single node that breaks C.  The SPF would have to be one of CA, CB and CC, but:
            //
            //    if we remove CA, we still have the cycle E ... F - CC ... CB - E;
            //    if we remove CB, we still have the cycle D ... E ... F - CC ... CB ... CA - D;
            //    if we remove CC, we still have the cycle D ... E - CB ... CA - D.
            return -1;
        }

        if (cycleNodesConnectedToComponentOccurrences.size() == 1)
        {
            assert(cycleNodesConnectedToComponentOccurrences.front().numOccurences > 1);
            //            ...─B─...─C─...          ┐
            //                 \   /               ├ Component's only point of contact with C is CA, but 
            //                  \ /                ┘ at least two nodes in the component are joined to CA.
            // ┌───────────...──CA──...──────────┐ ┐
            // │                                 │ ├ Cycle C
            // └─────────────────────────────────┘ ┘        
            // The cycle B ... C - CA - B can (and must) be broken by removing the connected cycle node, CA.

            cycleNodesConnectedToComponentOccurrences.front().node->numComponentCyclesBreaks++;
            continue;
        }

        // After this point, precisely two cycle nodes are connected to a node in this component.
        assert(cycleNodesConnectedToComponentOccurrences.size() == 2);
        if (cycleNodesConnectedToComponentOccurrences[0].numOccurences > 1 && cycleNodesConnectedToComponentOccurrences[1].numOccurences > 1)
        {
            //   ...─C─...─D──........──E─...─F─...   ┐
            //        \   /             \   /         ├ Component has exactly two points of contact with C (CA and CB),
            //         \ /               \ /          ┘ but each of CA and CB is connected to at least two points of component.
            // ┌──...──CA──...──────...──CB──...────┐ ┐
            // │                                    │ ├ Cycle C
            // └────────────────────────────────────┘ ┘        
            // We'd need to remove both cycle nodes to break this cycle - can't be done:
            //
            //    if we remove CA, we'd still have the cycle CB - E ... F - CB;
            //    if we remove CB, we'd still have the cycle CA - C ... D - CA.
            return -1;
        }
        if (cycleNodesConnectedToComponentOccurrences[0].numOccurences == 1 && cycleNodesConnectedToComponentOccurrences[1].numOccurences == 1)
        {
            //               D──....──E             ┐
            //               │        │             ├ Component has exactly two points of contact with C (CA and CB),
            //               │        │             ┘ and each of CA and CB is connected to precisely one node of component.
            // ┌───────...──CA──...──CB──...──────┐ ┐
            // │                                  │ ├ Cycle C
            // └──────────────────────────────────┘ ┘        
            // We'd need to remove both cycle nodes to break this cycle - can't be done:
            //
            //    if we remove CA, we'd still have the cycle CB ... E ... F ... CB;
            //    if we remove CB, we'd still have the cycle CA ... C ... D ... CA;
            //
            // Removing *either* of the cycle nodes CA and CB will break the cycle C and the cycle
            // CA - D ... E - CB ... CA.
            cycleNodesConnectedToComponentOccurrences[0].node->numComponentCyclesBreaks++;
            cycleNodesConnectedToComponentOccurrences[1].node->numComponentCyclesBreaks++;
            continue;
        }
        // Precisely one of the two cycle nodes is joined to more than one node in this component.
        // Removing that cycle node (in this case, CB, wlog) will break the cycle CB - E ... F - CB in this component.
        //
        //               D──....─E─...─F             ┐
        //               │        \   /              ├ Component has exactly two points of contact with C (CA and CB),
        //               │         \ /               ┘ and CA (wlog) is attached to exactly one node of component; CB to at least two.
        // ┌───────...──CA───...───CB──...─────────┐ ┐
        // │                                       │ ├ Cycle C
        // └───────────────────────────────────────┘ ┘        
        assert((cycleNodesConnectedToComponentOccurrences[0].numOccurences > 1) != (cycleNodesConnectedToComponentOccurrences[1].numOccurences > 1));
        if (cycleNodesConnectedToComponentOccurrences[0].numOccurences > 1)
            cycleNodesConnectedToComponentOccurrences[0].node->numComponentCyclesBreaks++;
        else
        {
            cycleNodesConnectedToComponentOccurrences[1].node->numComponentCyclesBreaks++;
        }

    }

    int minNodeId = -1;
    for (auto node : cycle)
    {
        if (node->numComponentCyclesBreaks == numComponentsNeedToBreak)
        {
            // Removal of this node will break every cycle.
            if (minNodeId == -1 || node->id < minNodeId)
                minNodeId = node->id;
        }
    }
    
    return minNodeId;
}

int main(int argc, char* argv[])
{
    // Fairly easy one, though shows a flaw with (purely) randomised testcase
    // generation: I put some assertions in some of the parts of the code
    // to ensure they were being exercised, but for some of these, none of 100'000(!)
    // randomised tests exercised the logic - manual creation of tests to exercise
    // the bits of logic took only a few minutes.
    //
    // Still passed first time, though - maybe the Problem Tester ran into the same
    // problem I did and ended up with really weak testcases XD
    //
    // QUICK EXPLANATION
    //
    // There is a simple, well-known O(N) algorithm for determining if a graph G has a 
    // cycle, and returning some arbitrary one C if there is.  If there is no cycle
    // C, there can be no SPF, and so we're done.
    //
    // Otherwise, the SPF *must* be a node in the cycle C (removing the SPF must
    // stop C from being a cycle).  By removing the cycle C from G, we obtain a bunch
    // of connected components, and by examining these components their points-of-contact
    // with nodes in C in the original graph, we can deduce, using fairly simple rules, 
    // which nodes in C must be removed to ensure that there are no cycles in G.
    // 
    // LONGER EXPLANATION
    //
    // The algorithm for finding a cycle is detailed here: https://en.wikipedia.org/wiki/Cycle_(graph_theory)#Cycle_detection
    // The cycle C returned by findACycle is more or less arbitrary, but whether it is a huge cycle
    // encompassing all N nodes, or teeny-weeny little 3-cycle triangle, the SPF *must* be a node in C
    // in order to leave no cycles when that SPF is removed.  We need to account for the fact that the
    // initial graph may not be connected, so we extract all connected components (getComponents) and check
    // each one for a cycle.  If no cycles are found anywhere, there is no SPF, and we're done.
    //
    // We then remove the cycle C to get G - C and examine the components that are left, and how each such remaining component
    // touches C in the original graph G (see cycleNodesConnectedToComponentOccurrences); at this point, reading the
    // code might be the best explanation for all the cases we need to consider - it's got ASCII Art and everything!
    // 
    // One small note - the case where a cycle C has additional connections between nodes in the cycle that aren't consecutive
    // in the cycle needs special attention: I side-step this by inserting a "synthetic" node between each
    // pair of such nodes (max O(M) of these) and breaking the original edge between them: then in G - C each synthetic node
    // is a component, and the algorithm deals with them in the normal way.  See addSyntheticEdgesBetweenConnectedNonConsecutiveCycleNodes.
    //
    // The whole algorithm is O(N).
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int numNodes = read<int>();
        const int numEdges = read<int>();

        vector<Node> nodes;
        nodes.reserve(numNodes + numEdges); // We might be creating up to numEdges synthetic nodes.
        nodes.resize(numNodes);
        for (int i = 0; i < numNodes; i++)
        {
            nodes[i].id = (i + 1);
        }

        for (int i = 0; i < numEdges; i++)
        {
            const int u = read<int>() - 1;
            const int v = read<int>() - 1;

            nodes[u].neighbours.push_back(&(nodes[v]));
            nodes[v].neighbours.push_back(&(nodes[u]));
        }

        cout << findSinglePointOfFailure(nodes) << endl;
    }

    assert(cin);
}
