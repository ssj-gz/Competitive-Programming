#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

struct Node
{
    Node* parent = nullptr;
    vector<Node*> children;
    int r = -1;
    int id = -1;

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

bool canMakeRight(Node* node, Node* parent, int maxRInParents, const vector<bool>& nodeIdsThatCanChange)
{
    if (parent && parent->r > node->r && !nodeIdsThatCanChange[node->id])
        return false;

    if (nodeIdsThatCanChange[node->id])
    {
        //cout << "Made a change - node " << (node->id + 1) << " from " << node->r << " to " << maxRInParents << endl;
        node->r = maxRInParents;
    }

    bool canMakeThisSubtreeRight = true;
    for (const auto child : node->children)
    {
        canMakeThisSubtreeRight = (canMakeThisSubtreeRight && canMakeRight(child, node, max(maxRInParents, node->r), nodeIdsThatCanChange));
    }
    return canMakeThisSubtreeRight;
}

int main()
{
    int n;
    cin >> n;

    vector<Node> nodes(n);
    vector<int> r(n);
    for (int i = 0; i < n; i++)
    {
        cin >> r[i];
        nodes[i].id = i;
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

        //cout << "numChanges: " << numChanges << " choices:" << allChoices.size() << endl;
        for (const auto& choice : allChoices)
        {
            for (int i = 0; i < n; i++)
            {
                nodes[i].r = r[i];
            }

            //cout << "Checking if can make right" << endl;
            if (canMakeRight(rootNode, nullptr, 0, choice))
            {
                //cout << "Wee" << endl;
                for (int i = 0; i < n; i++)
                {
                    //cout << "Node: " << (i + 1) << " parent: " << (nodes[i].parent ? nodes[i].parent->id + 1 : -1) << " r: " << nodes[i].r << endl;
                }
                cout << numChanges << endl;
                return 0;
            }

        }
        //cout << endl;

        numChanges++;


    }

}
