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
    bool isAdjacentToInCycle(Node* otherNode)
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
        // O(number of distinct nodes in set).
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
    // A connected graph over N nodes will have at least
    // N - 1 edges.
    // It is a tree if and only if it has precisely N - 1 edges.
    return numEdges != component.size() - 1;
}

void addSyntheticEdgesBetweenNonAdjacentCycleNodes(vector<Node>& nodes)
{
    // Introduce synthetic nodes so that no two non-consecutive cycle
    // nodes are connected.
    vector<std::pair<Node*, Node*>> directEdgesToReplace;
    for (auto& node : nodes)
    {
        if (!node.isInCycle)
            continue;

        for (auto neighbour : node.neighbours)
        {
            if (!neighbour->isInCycle)
                continue;
            if (node.isAdjacentToInCycle(neighbour))
                continue;

            if (node.id < neighbour->id) // Ensure we don't process the same "pair" more than once.
            {
                directEdgesToReplace.push_back({&node, neighbour});
            }
        }

        node.neighbours.erase(remove_if(node.neighbours.begin(), node.neighbours.end(), 
                                        [&node](const auto neighbour) 
                                        { 
                                            return node.isAdjacentToInCycle(neighbour); 
                                        }),
                              node.neighbours.end());
    }
    // Having removed edges between pairs of non-adjacent cycle nodes, add the synthetic node
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
    // not adjacent in C without special casing.
    addSyntheticEdgesBetweenNonAdjacentCycleNodes(nodes);

    // Get the list of components remaining after we've removed C.
    const vector<vector<Node*>> components = getComponents(nodes);

    for (const auto& component : components)
    {
        if (componentHasCycle(component))
        {
            // A cycle exists that has no nodes in common with C;
            // no way of removing a single node and breaking both
            // C and this cycle.
            return -1;
        }
    }

    // None of the components has a cycle in G-C.
    NodeMultiSet cycleNodesConnectedToComponent(nodes.size());
    int numComponentsNeedToBreak = 0;
    for (const auto& component : components)
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
            continue;
        }

        // This component has a cycle in G but not in G-C - removal of the single point of failure must
        // break the cycle in this component.
        numComponentsNeedToBreak++;

        if (cycleNodesConnectedToComponentOccurrences.size() >= 3)
        {
            // 3 distinct cycle nodes connected; cannot be broken by any single node that breaks C.
            return -1;
        }

        if (cycleNodesConnectedToComponentOccurrences.size() == 1)
        {
            // Can (and must) be broken by removing the connected cycle node.
            assert(cycleNodesConnectedToComponentOccurrences.front().numOccurences > 1);
            cycleNodesConnectedToComponentOccurrences.front().node->numComponentCyclesBreaks++;
            continue;
        }

        // After this point, precisely two cycle nodes are connected to a node in this component.
        assert(cycleNodesConnectedToComponentOccurrences.size() == 2);
        if (cycleNodesConnectedToComponentOccurrences[0].numOccurences > 1 && cycleNodesConnectedToComponentOccurrences[1].numOccurences > 1)
        {
            // We'd need to remove both cycle nodes to break this cycle - can't be done.
            return -1;
        }
        if (cycleNodesConnectedToComponentOccurrences[0].numOccurences == 1 && cycleNodesConnectedToComponentOccurrences[1].numOccurences == 1)
        {
            // Removing either cycle node will break the cycle.
            cycleNodesConnectedToComponentOccurrences[0].node->numComponentCyclesBreaks++;
            cycleNodesConnectedToComponentOccurrences[1].node->numComponentCyclesBreaks++;
            continue;
        }
        // Precisely one of the two cycle nodes is joined to more than one node in this component.
        // Removing that cycle node will break the cycle in this component.
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
            if (minNodeId == -1 || node->id < minNodeId)
                minNodeId = node->id;
        }
    }
    
    return minNodeId;
}

int main(int argc, char* argv[])
{
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

        const auto singlePointOfFailure = findSinglePointOfFailure(nodes);
        cout << singlePointOfFailure << endl;
    }

    assert(cin);
}
