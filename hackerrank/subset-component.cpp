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
        int numComponents = 64;
        for (const auto component : componentsSoFar)
        {
            numComponents -= (countBits(component) - 1);
        }
        totalConnectedComponents += numComponents;
        return;
    }
    const auto node = edges[edgeIndex];
    vector<uint64_t> nextComponents;
    uint64_t componentIncludingNode = node;
    for (const auto component : componentsSoFar)
    {
        if ((component & node) != 0)
        {
            componentIncludingNode |= component;
        }
        else
        {
            nextComponents.push_back(component);
        }
    }
    if (componentIncludingNode != 0)
        nextComponents.push_back(componentIncludingNode);
    countConnectedComponentsForSubsets(edges, edgeIndex + 1, nextComponents, totalConnectedComponents);

    countConnectedComponentsForSubsets(edges, edgeIndex + 1, componentsSoFar, totalConnectedComponents);
}

int main()
{
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
