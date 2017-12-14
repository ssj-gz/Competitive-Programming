#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

struct Node
{
    Node* parent = nullptr;
    vector<Node*> children;
    int r = -1;

    vector<Node*> neighbours;
};

void generateChoices(vector<bool>& choicesSoFar, int index, int numChoicesRemaining, vector<vector<bool>>& results)
{
    //cout << "generateChoices: index: " << index << endl;
    if (index == choicesSoFar.size())
    {
        results.push_back(choicesSoFar);
        return;
    }
    assert(index < choicesSoFar.size());
    if (choicesSoFar.size() - index > numChoicesRemaining)
    {
        generateChoices(choicesSoFar, index + 1, numChoicesRemaining, results);
    }
    //cout << "trying with thing" << endl;
    if (numChoicesRemaining > 0)
    {
        choicesSoFar[index] = true;
        generateChoices(choicesSoFar, index + 1, numChoicesRemaining - 1, results);
        choicesSoFar[index] = false;
    }
}

void correctDFS(Node* node, Node* parent)
{
    node->parent = parent;
    for (const auto neighbour : node->neighbours)
    {
        if (neighbour != parent)
        {
            node->children.push_back(neighbour);
            correctDFS(neighbour, node);
        }

    }
    node->neighbours.clear();
}

int main()
{
    int n;
    cin >> n;

    vector<Node> nodes(n);
    for (int i = 0; i < n; i++)
    {
        cin >> nodes[i].r;
    }

    for (int i = 0; i < n - 1; i++)
    {
        int x, y;
        cin >> x >> y;
        x--;
        y--;

        nodes[x].neighbours.push_back(&nodes[y]);
        nodes[y].neighbours.push_back(&nodes[x]);
    }
    assert(cin);

    Node* rootNode = &(nodes.front());
    correctDFS(rootNode, nullptr);

    int numChanges = 0;
    while (true)
    {
        vector<bool> emptyChoices(n, false);
        vector<vector<bool>> allChoices;
        generateChoices(emptyChoices, 0, numChanges, allChoices);

        cout << "numChanges: " << numChanges << " choices:" << allChoices.size() << endl;
        for (const auto& blah : allChoices)
        {
            for (const auto x : blah)
            {
                cout << x << " ";
            }
            cout << endl;
        }
        cout << endl;

        numChanges++;


    }

}
