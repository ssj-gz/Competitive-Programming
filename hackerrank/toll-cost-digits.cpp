// Simon St James (ssjgz) 2017-09-29 14:20
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main()
{
    // Ouch - a hard one, this, I thought, and I'm quite surprised by the high completion rate (~44.05% at the time of writing).
    // So: we are only interested in the last digit of costs, which strongly hints that we should be using arithmetic modulo 10
    // throughout.   Let P = ei1, ei2, ... eik be a path (eij are edges; since there can be multiple edges between nodes,
    // we must express paths in terms of edges, not nodes).  Let C(eij) be the cost of traversing edge eij.  Let CM(P) be the cost of
    // the path P, modulo 10; then CM(P) = (sum j = 1...k {eij}) % 10.  
    //
    // Let ~e denote the cost of traversing the edge e backwards; from the problem statement, we have C(~e) = 1000 - C(e).
    // Let ~P be the reverse of P i.e. ~P = ~eik, ~eik-1, ... ~ei1; then CM(~P) = (sum j = 1...k {~eij}) % 10 = 
    // (k * 1000 + sum j = 1...k {-eij}) % 10 = (k * 1000 + -CM(P)) % 10 = -CM(P) % 10.  Thus, we see that traversing a path P
    // from x to y and then traversing the reverse path ~P from y to x is a path with 0 cost!  If P1 and P2 are paths, then let
    // P1.P2 be the concatenation of these paths; then it's easily shown that CM(P1.P2) = CM(P1) + CM(P2).
    //
    // Anyway, the problem initially seems very hard: how can we possibly compute all pairs (x,y) with a path P from x to y with
    // CM(P) = d? It's slightly simpler than it looks.  For the time being, let's restrict ourselves to the case where the 
    // graph is connected; the case of multiple components poses no additional difficulties.
    //
    // Here is the trick: pick any node as a "root" node and call it A, say.  Define N(A, d) to be, for digit d in (0..9),
    // the set of nodes x for which there is a path P from A to x such that CM(P) = d.  I claim that, for a pair of nodes
    // x and y and digit d, there is a path Pxy with C(Pxy) = d if and only if there are dx and dy such that
    // x is in N(A, dx) and y is in N(A, dy) and dy - dx = d (mod 10, of course) i.e. that  there is a path of cost d from
    // x to y if and only if there is a path Py from A to y and a path Px from a to x with CM(Py) - CM(Px) = d.
    //
    // Assume first direction i.e. that there is a path Pxy from x to y with CM(Pxy) = d.  Let Px be any path from A to x.
    // Then Px.Pxy is a path from A to y, and we have that CM(Px.Pxy) - CM(Px) = CM(Px) + CM(Pxy) - CM(Px) = CM(Pxy) = d, 
    // as required.
    //
    // Assume second direction i.e. that there are paths Px from A to x and Py from A to y with d = CM(Py) - CM(Px).
    // Then ~Px.Py is a path from x to y and CM(~Px.Py) = CM(~Px) + CM(Py) = CM(Py) - CM(Px) = d, as required.
    //
    // So: if we are able to construct the set N(A, d) for each d, we can easily compute the number of pairs x,y where
    // there is a path P from x to y with CM(P) = d: let xCost, yCost (in 0..9) be such that yCost - xCost = d.
    // Then every pair consisting of a node x from N(A, xCost) and a node y from N(A, yCost) will have a path of cost d,
    // and likewise any pair x,y with a path of cost d will be in some N(A, xCost) and N(A, yCost) (respectively), with yCost - xCost = d.
    // For a given xCost and yCost, the number of such pairs is |N(A, xCost)| x |N(A, yCost)|.  Actually, that's not quite right:
    // if N(A, xCost) and N(A, yCost) share any nodes, we will overcount (the problem statement states that the pairs must 
    // have two different nodes), so we'll be overcounting by the number of nodes that belong to both N(A, xCost) and N(A, yCost).
    // So we can find the number of pairs with a path of cost d by finding all xCost,yCost with yCost - xCost = d and
    // adding |N(A, xCost)| x |N(A, yCost)| - number of nodes in both N(A, xCost) and N(A, yCost).
    //
    // There's a slightly more serious problem, though: what if there were two different pairs (xCost1, yCost1), (xCost2, yCost2)
    // with yCost1 - xCost1 = yCost2 - xCost2 = d, and there was nodes x and y where x was in both N(A, xCost1) and N(A, xCost2)
    // and y was in both N(A, yCost1) and N(A, yCost2)? Then we would count the pair x,y as having a path of length d more than 
    // once! This is the last hurdle, and it has a reasonably simple solution.  As it happens, for any costs c1 and c2, we have 
    // that either N(A, c1) = N(A, c2), or N(A, c1) and N(A, c2) have no vertices in common.
    //
    // To see this, assume wlog that c2 > c1, and let k = c2 - c1.  I claim that a vertex x is in both N(A, c1) and N(A, c2) if and only
    // if A has a self loop of cost k and a path to x of length c1.
    //
    // Assume first direction i.e. that there is vertex x that is in both N(A, c1) and N(A, c2).  Then there is a path P1 from A to x
    // with CM(P1) = c1 and a path P2 from A to x with CM(P2) = c2. But then the path P2.~P1 is a path from A to A, and its
    // cost is CM(P2.~P1) = CM(P2) - CM(P1) = c2 - c1 = k.
    //
    // Assume second direction i.e. that A has a self loop P with CM(P) = k and a path Px from A to x with CM(Px) = c1. Then
    // the path P.Px is a path from A to x and CM(P.Px) = CM(P) + CM(Px) = k + c1 = c2, so x belongs to N(A, c2).
    //
    // Thus, if we have already processed (xCost1, yCost1), then we don't need to (and in fact, shouldn't!) process (xCost2, yCost2) if A has a self-loop
    // of length k and xCost2 - xCost1 = k and yCost2 - yCost1 = k; adhering to this allows us to solve this overcounting problem!
    //
    // There's also another bonus: counting the number of vertices in both N(A, xCost) and N(A, yCost) is easy: they are disjoint if and only if
    // there is no self-loop from A to A of cost yCost - xCost, and equal if and only if there is one.  So in fact, rather than computing
    // N(A, cost), we need only count the number of nodes in it (numJunctionsReachableFromRootWithCost[cost]).
    //
    // And that's about it: we still have to compute N(A, d) for each d, but this is easily done just by doing a BFS from A over a meta-graph
    // of digits x nodes (isJunctionIndexReachableFromRootWithCost).  Hooray!

    struct Junction;
    struct Road
    {
        Junction* junction;
        int cost = 0;
    };
    struct Junction
    {
        vector<Road> neighbours;
        int index = 0;
        bool reached = false;
    };

    ios::sync_with_stdio(false);

    int n, e;
    cin >> n >> e;

    vector<Junction> junctions(n);
    for (int i = 0; i < n; i++)
    {
        junctions[i].index = i;
    }

    for (int i = 0; i < e; i++)
    {
        int x, y, r;
        cin >> x >> y >> r;
        x--;
        y--;
        r %= 10;

        Road roadXY;
        roadXY.cost = r;
        roadXY.junction = &(junctions[y]);
        junctions[x].neighbours.push_back(roadXY);

        // Add reverse edge.
        Road roadYX;
        roadYX.junction = &(junctions[x]);
        roadYX.cost = (10 - r ) % 10;
        junctions[y].neighbours.push_back(roadYX);
    }

    int64_t numPairsWithCost[10] = {};

    // isJunctionIndexReachableFromRootWithCost[d][index] is true if and only if there
    // is a path P from componentRoot to junction with index index and CM(P) = d.
    // NB: We use char instead of bool as bool is *vastly* slower.
    // NB2: it looks like we might have to re-initialise this for each component of the 
    // graph, but that would be O(N ^ 2) and it is in fact non-necessary: a given
    // Junction can not be in more than one component i.e. reachable from more than one
    // component root!
    vector<vector<char>> isJunctionIndexReachableFromRootWithCost(10, vector<char>(n));

    for (int componentRootIndex = 0; componentRootIndex < n; componentRootIndex++)
    {
        if (junctions[componentRootIndex].reached)
            continue;
        Junction* componentRoot = &(junctions[componentRootIndex]);

        int64_t numJunctionsReachableFromRootWithCost[10] = {};

        struct JunctionAndCost
        {
            JunctionAndCost(Junction* junction, int cost)
                : junction{junction}, cost{cost}
            {
            }
            Junction *junction = nullptr;
            int cost = 0;
        };

        // Use a BFS to compute numJunctionsReachableFromRootWithCost.
        JunctionAndCost rootJunctionAndCost(componentRoot, 0);
        vector<JunctionAndCost> junctionsAndCostsToExplore = { rootJunctionAndCost };
        while (!junctionsAndCostsToExplore.empty())
        {
            vector<JunctionAndCost> nextJunctionsAndCostsToExplore;
            for (const auto& junctionAndCost : junctionsAndCostsToExplore)
            {
                auto junction = junctionAndCost.junction;
                const auto costToReachJunction = junctionAndCost.cost;
                if (isJunctionIndexReachableFromRootWithCost[costToReachJunction][junction->index])
                    continue;
                isJunctionIndexReachableFromRootWithCost[costToReachJunction][junction->index] = true; // Mark as explored so we don't re-explore it!
                junction->reached = true; // This junction is in this component, so don't pick it as the root of another component!
                numJunctionsReachableFromRootWithCost[costToReachJunction]++;

                for (const auto& road : junction->neighbours)
                {
                    auto neighbourJunction = road.junction;
                    const auto costToReachNeighbour = (costToReachJunction + road.cost) % 10;
                    if (!isJunctionIndexReachableFromRootWithCost[costToReachNeighbour][neighbourJunction->index])
                    {
                        JunctionAndCost neighbourJunctionAndCost(neighbourJunction, costToReachNeighbour);
                        nextJunctionsAndCostsToExplore.push_back(neighbourJunctionAndCost);
                    }
                }
            }
            junctionsAndCostsToExplore = nextJunctionsAndCostsToExplore;
        }

        vector<int> rootSelfLoopCosts;
        for (int cost = 0; cost <= 9; cost++)
        {
            if (isJunctionIndexReachableFromRootWithCost[cost][componentRoot->index])
            {
                rootSelfLoopCosts.push_back(cost);
            }
        }

        for (int cost = 0; cost <= 9; cost++)
        {
            int xCost = 0;
            int yCost = xCost + cost;
            const bool junctionsReachableWithXAndYCostsAreEqual = (find(rootSelfLoopCosts.begin(), rootSelfLoopCosts.end(), cost) != rootSelfLoopCosts.end());

            int64_t numPairs = 0;
            bool alreadyHandledEquivalent = false;
            bool isEquivalentXCostHandled[10] = {};
            while (xCost <= 9)
            {
                if (!isEquivalentXCostHandled[xCost])
                {
                    numPairs += numJunctionsReachableFromRootWithCost[yCost] * numJunctionsReachableFromRootWithCost[xCost];
                    // The product above will count pairs where the junctions are the same; subtract this overcount.
                    // The overCount is the number of vertices that are reachable with both xCost and yCost.
                    const int64_t overCount = (junctionsReachableWithXAndYCostsAreEqual ? numJunctionsReachableFromRootWithCost[xCost] : 0);
                    numPairs -= overCount;

                    // Make sure the we don't handle "equivalent" pairs of xCost and yCost (it would lead to over-counting).
                    for (const auto rootSelfLoopCost : rootSelfLoopCosts)
                    {
                        isEquivalentXCostHandled[(xCost + rootSelfLoopCost) % 10] = true;
                    }
                }

                xCost++;
                yCost = (yCost + 1) % 10;
            }
            numPairsWithCost[cost] += numPairs;
        }
    }
    for (const auto numPairs : numPairsWithCost)
    {
        cout << numPairs << endl;
    }
}
