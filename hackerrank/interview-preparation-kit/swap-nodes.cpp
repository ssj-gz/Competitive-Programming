#include <iostream>
#include <vector>

#include <cassert>

using namespace std;

struct Node
{
    int index = -1;
    Node* leftNode = nullptr;
    Node* rightNode = nullptr;
};

void swapNodes(Node* currentNode, const int currentDepth, const int swapAtDepthMultiple, vector<int>& visitedNodeIds)
{
    if (currentNode == nullptr)
        return;

    if ((currentDepth % swapAtDepthMultiple) == 0)
    {
        std::swap(currentNode->leftNode, currentNode->rightNode);
    }

    swapNodes(currentNode->leftNode, currentDepth + 1, swapAtDepthMultiple, visitedNodeIds);
    visitedNodeIds.push_back(currentNode->index);
    swapNodes(currentNode->rightNode, currentDepth + 1, swapAtDepthMultiple, visitedNodeIds);
}

int main()
{
    int n;
    cin >> n;

    vector<Node> nodes(n);

    for (int i = 0; i < n; i++)
    {
        nodes[i].index = (i + 1);

        int a;
        cin >> a;

        int b;
        cin >> b;

        if (a != -1)
        {
            nodes[i].leftNode = &(nodes[a - 1]);
        }
        if (b != -1)
        {
            nodes[i].rightNode = &(nodes[b - 1]);
        }
    }

    auto rootNode = &(nodes.front());

    int t;
    cin >> t;

    vector<int> queries(t);
    for (int i = 0; i < t; i++)
    {
        cin >> queries[i];
    }

    assert(cin);

    for (const auto k : queries)
    {
        vector<int> visitedNodeIds;
        swapNodes(rootNode, 1, k, visitedNodeIds);

        for (int i = 0; i < visitedNodeIds.size(); i++)
        {
            cout << visitedNodeIds[i];
            if (i + 1 != visitedNodeIds.size())
            {
                cout << " ";
            }
        }
        cout << endl;
    }
}
