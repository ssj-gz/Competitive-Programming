// Simon St James (ssjgz) - 2019-07-10
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

struct Node
{
    vector<Node*> neighbours;
    int componentNum = -1;

};

int findNumInComponentAndMarkAll(Node* node, int componentNum)
{
    int numInComponent = 1;
    vector<Node*> nodesToExplore = { node };
    node->componentNum = componentNum;

    while (!nodesToExplore.empty())
    {
        vector<Node*> nextNodesToExplore;
        for (auto nodeToExplore : nodesToExplore)
        {
            for (auto neighbour : nodeToExplore->neighbours)
            {
                if (neighbour->componentNum == -1)
                {
                    neighbour->componentNum = componentNum;
                    nextNodesToExplore.push_back(neighbour);
                    numInComponent++;
                }
            }
        }

        nodesToExplore = nextNodesToExplore;
    }

    return numInComponent;
}

int main()
{
    int N;
    cin >> N;

    vector<Node> nodes(2 * N);
    for (int i = 0; i < N; i++)
    {
        int g, b;
        cin >> g >> b;
        // Make 0-relative.
        g--;
        b--;

        nodes[g].neighbours.push_back(&(nodes[b]));
        nodes[b].neighbours.push_back(&(nodes[g]));
    }

    vector<int> componentSizes;
    for (auto& node : nodes)
    {
        if (node.componentNum != -1)
            continue;

        const auto numInNewComponent = findNumInComponentAndMarkAll(&node, componentSizes.size());
        if (numInNewComponent != 1)
            componentSizes.push_back(numInNewComponent);
    }

    const auto numInMinComponent = *min_element(componentSizes.begin(), componentSizes.end());
    const auto numInMaxComponent = *max_element(componentSizes.begin(), componentSizes.end());

    cout << numInMinComponent << " " << numInMaxComponent << endl;

}
