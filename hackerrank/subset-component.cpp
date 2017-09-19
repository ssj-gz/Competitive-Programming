// Simon St James (ssjgz) - 2017-09-19
#include <iostream>
#include <vector>

using namespace std;

int countBits(uint64_t n)
{
    int numBits = 0;
    while (n > 0)
    {
        if ((n & 1) == 1)
            numBits++;

        n >>= 1;
    }
    return numBits;
}

void countConnectedComponentsForSubsets(const vector<uint64_t>& edges, int edgeIndex, const vector<uint64_t>& componentsSoFar, uint64_t& totalConnectedComponents)
{
    if (edgeIndex == edges.size())
    {
        // Finished a subset.  The components are mutually bitwise-disjoint i.e. ci | cj == 0 for all i != j, and ci contains vertex v
        // if and only if (ci & v) != 0.
        int numComponents = 64;
        for (const auto component : componentsSoFar)
        {
            // Number of vertices in component == # bits - 1.
            numComponents -= (countBits(component) - 1);
        }
        totalConnectedComponents += numComponents;
        return;
    }
    const auto edge = edges[edgeIndex];
    vector<uint64_t> nextComponents;
    uint64_t componentIncludingEdge = edge;
    for (const auto component : componentsSoFar)
    {
        if ((component & edge) != 0)
        {
            // The component includes a vertex which is an endpoint of this edge.
            componentIncludingEdge |= component;
        }
        else
        {
            nextComponents.push_back(component);
        }
    }
    if (componentIncludingEdge != 0)
        nextComponents.push_back(componentIncludingEdge);
    // Recurse after incorporating this edge.
    countConnectedComponentsForSubsets(edges, edgeIndex + 1, nextComponents, totalConnectedComponents);

    // Recurse after not incorporating this edge.
    countConnectedComponentsForSubsets(edges, edgeIndex + 1, componentsSoFar, totalConnectedComponents);
}

int main()
{
    // Fairly easy one, but yet again, schoolboy errors XD
    // So: the d's represent edges between vertices, and  vertices i & j are connected by an edge
    // if and only if there is a k such that BIT(d[k], i) == 1 && BIT(d[k], j) == 1.
    // Imagine that there is a subset D' of D such that vertices C = {vi1, vi2, ... , vim} are in the 
    // same connected compnent.  Pick another vertex v.  How do we tell if v is in C?
    // It will be in C if and only if there is some c in C, j and l with d[j], d[l] in D' and BIT(d[j], c) == 1 == BIT(d[l], v).
    // Now, imagine that we or'd all edges in D' together to get dbar: then d would have a 1 at index p if and only if 
    // there is some q (d[q] in D') such that BIT(d[q], p) == 1.  Thus, v is in C if and only if BIT(dbar, v) == 1.
    // So: we can represent a component C by a single integer dbar(C) representing the or of all the d's comprising it, and then tell
    // if a new edge e is part of that component or not just by testing whether an endpoint (ve1 or ve2) of e is in
    // C which we can tell by seeing if bit ve1 or ve2 is set in dbar(C).   
    // So, for each edge e in d, we either incorporate it or not, and update the set of components that arise from incorporating it.
    // Doing this for all e in d is the same as doing it for all subsets of d, which is what we want.
    int n;
    cin >> n;
    vector<uint64_t> d(n);
    for (int i = 0; i < n; i++)
    {
        cin >> d[i];
    }
    vector<uint64_t> components;
    uint64_t totalConnectedComponents = 0;
    countConnectedComponentsForSubsets(d, 0, components, totalConnectedComponents);

    cout << totalConnectedComponents << endl;
}
