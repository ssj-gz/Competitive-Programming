#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include <limits>
using namespace std;

struct Node;
struct Edge
{
    Edge(Node* otherNode, int timeToDestroy)
        : otherNode(otherNode),
        timeToDestroy(timeToDestroy)
    {

    }
    Node *otherNode;
    int timeToDestroy = -1;

};
struct Node
{
    vector<Edge> neighbours;
    bool isMachine = false;
};

struct Result
{
    bool leadsToMachine = false;
    int minTimeToDisconnectMachine = numeric_limits<int>::max();
};

long minDisconnectionTime = 0;
Result disconnectMachines(Node* startNode, Node* parentNode)
{
    // A node that is a machine can trivially reach a machine :)
    bool hasReachableMachineDescendent = (startNode->isMachine);
    int mostExpensiveChildMachineDisconnectTime = 0;
    for (auto edge : startNode->neighbours)
    {
        Node *childNode = edge.otherNode;
        if (childNode == parentNode)
            continue;

        Result childResult = disconnectMachines(childNode, startNode);
        if (!childResult.leadsToMachine)
        {
            // Discard children that do not lead to a machine; they have no effect
            // on the outcome.
            continue;
        }
        hasReachableMachineDescendent = true;
        // Incorporate the time to destroy road from startNode to the child into
        // the minimum time to disconnect from the machine the child leads to.
        childResult.minTimeToDisconnectMachine = min(childResult.minTimeToDisconnectMachine,
                edge.timeToDestroy);
        // "Disconnect" each child machine by default; this will pairwise disconnect all child machines
        // from each other, and from startNode if startNode is a machine.
        minDisconnectionTime += childResult.minTimeToDisconnectMachine;
        mostExpensiveChildMachineDisconnectTime = max(mostExpensiveChildMachineDisconnectTime,
                childResult.minTimeToDisconnectMachine);
    }
    if (!hasReachableMachineDescendent)
    {
        Result result;
        result.leadsToMachine = false;
        return result;
    }
    Result result;
    result.leadsToMachine = true;
    if (!startNode->isMachine)
    {
        // We only needed to disconnect all but one (the most expensive one) of the child machines,
        // but we disconnected them all;  correct for this.
        minDisconnectionTime -= mostExpensiveChildMachineDisconnectTime;
        // We didn't disconnect the machine with the highest time-cost to disconnect,
        // so we need to make a note of this for our parent node.
        result.minTimeToDisconnectMachine = mostExpensiveChildMachineDisconnectTime;
    }
    return result;
}

int main() {
    int N, K;
    cin >> N >> K;
    vector<Node> allNodes(N);
    for (int i = 0; i < N - 1; i++)
    {
        int x, y, z;
        cin >> x >> y >> z;
        allNodes[x].neighbours.push_back(Edge(&(allNodes[y]), z));
        allNodes[y].neighbours.push_back(Edge(&(allNodes[x]), z));
    }
    for (int i = 0; i < K; i++)
    {
        int machineNode;
        cin >> machineNode;
        allNodes[machineNode].isMachine = true;
    }
    // Liked this one :) Let v be a vertex.  Assume we have some DFS-based
    // algorithm which explores the graph, and let c1, c2 ... cm be the children of 
    // v in this DFS.  Assume further that this algorithm, upon completion for
    // child c of a node x, guarantees the following:
    //  1) Cuts are made to ensure that the set of machines reachable by all paths P beginning
    //     with xc in the remaining graph is either empty, or consists of just one machine
    //     (call it M);
    //  2) We know the minimum time-cost of cutting M (if it exists) from x (which will be one of the roads in
    //     P).
    // How do we proceed once we've explored all children ci of v? Well, if ci doesn't lead to 
    // a machine, we don't have to do anything with any descendents of paths beginning with vci
    // i.e. we don't have to make any disconnections of a child of the edge v-ci nor any descendent
    // of ci.  So let's assume without generality that all of the m ci lead to a machine; from 1)
    // the path Pi in the remaining graph beginning with v-ci leads to precisely one machine; call it
    // Mi.  If the set of ci is then empty (m == 0), then we are done, and note that if p is the 
    // parent of v, then any path beginning with pv cannot reach a machine.
    // Now: if v itself is a machine, then we must disconnect every Mi from v (which will also have the
    // effect of disconnecting each Mi from each Mj for each i != j, as all reachable descendent 
    // machines of v must travel through v if they are to reach each other).  Thus, making a cut
    // of one and only one road ri from each Pi is sufficiently to mutually disconnect all v<->Mi and
    // Mi<->Mj (i != j).  The choice of which ri to disconnect cannot affect future decisions, so
    // obviously pick the minimum ri for each Pi, which we know from 2).  Note that if p is the parent
    // of v, then any path beginning with pv leads to precisely one machine, that is, v itself; the
    // Mi are no longer reachable from v, and thus, neither from p.
    // Conversely, if v is *not* a machine, then we must only mutually disconnect each Mi from Mj, i != j,
    // which can be achieved by removing a road ri from just *m-1* of the Pis - again, it doesn't affect
    // future results which we choose, so choose the cheapest option - that is, if Pj has the highest
    // minimum time-cost of disconnecting v from Mj, then don't pick a road in Pj! Note that this time,
    // if p is the parent of v, then any path beginning with pv will reach exactly one machine:
    // that is, Mj.
    // And that's the algorithm: the steps for v ensure that all descendent machines of v (including
    // v itself, if v is a machine) are mutually disconnected, and also, with a little bit of minor
    // bookkeeping, fulfil conditions 1) and 2) above.  Hooray!
    Node  *startNode = nullptr;
    for (auto& node : allNodes)
    {
        if (node.isMachine)
        {
            startNode = &node;
            break;
        }
    }

    disconnectMachines(startNode, nullptr);
    cout << minDisconnectionTime << endl;

    return 0;
}

