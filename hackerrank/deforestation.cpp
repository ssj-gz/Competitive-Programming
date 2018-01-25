// Simon St James (ssjgz) - 2018-01-25
#include <iostream>
#include <vector>

using namespace std;

struct Node
{
    vector<Node*> neighbours;
    vector<Node*> children;
};

void correctParentChild(Node* currentNode, Node* parentNode)
{
    for (auto child : currentNode->neighbours)
    {
        if (child == parentNode)
            continue;

        currentNode->children.push_back(child);
        correctParentChild(child, currentNode);
    }

}

int findGrundyNum(Node* root)
{
    if (root->children.empty())
    {
        return 0;
    }

    if (root->children.size() == 1)
    {
        return 1 + findGrundyNum(root->children.front());
    }

    int grundyNum = 0;
    for (auto child : root->children)
    {
        grundyNum ^= (1 + findGrundyNum(child));
    }

    return grundyNum;
}

int main()
{
    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        int N;
        cin >> N;

        vector<Node> nodes(N);

        for (int i = 0; i < N - 1; i++)
        {
            int u, v;
            cin >> u >> v;
            u--;
            v--;


            nodes[u].neighbours.push_back(&(nodes[v]));
            nodes[v].neighbours.push_back(&(nodes[u]));
        }

        auto rootNode = &(nodes.front());
        correctParentChild(rootNode, nullptr);
        const auto grundyNum = findGrundyNum(rootNode);

        if (grundyNum == 0)
            cout << "Bob";
        else 
            cout << "Alice";
        cout << endl;
    }
}
