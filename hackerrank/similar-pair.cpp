#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

struct Node
{
    vector<Node*> children;
    int nodeNumber = -1;
    bool isRoot = true;
};


int numSimilarPairsBruteForce(Node* root, int k, vector<Node*>& ancestors)
{
    int numSimilarPairs = 0;
    for (const auto& ancestor : ancestors)
    {
        if (abs(root->nodeNumber - ancestor->nodeNumber) <= k)
        {
            numSimilarPairs++;
        }
    }
    ancestors.push_back(root);
    for (auto child : root->children)
    {
        numSimilarPairs += numSimilarPairsBruteForce(child, k, ancestors);

    }
    ancestors.pop_back();
    return numSimilarPairs;

}

int main()
{
    int n, k;
    cin >> n >> k;

    vector<Node> nodes(n);
    for (int i = 0; i < n; i++)
    {
        nodes[i].nodeNumber = i + 1;
    }

    for (int i = 0; i < n - 1; i++)
    {
        int parent, child;
        cin >> parent >> child;

        parent--;
        child--;

        nodes[parent].children.push_back(&(nodes[child]));
        nodes[child].isRoot = false;
    }

    Node* root = nullptr;
    for (auto& node : nodes)
    {
        if (node.isRoot)
        {
            assert(!root);
            root = &node;
        }
    }
    assert(root);

#define BRUTE_FORCE
#ifdef BRUTE_FORCE
    vector<Node*> ancestors;
    const int numSimilarPairsBruteForce = ::numSimilarPairsBruteForce(root, k, ancestors);
    cout << numSimilarPairsBruteForce << endl;
#endif

}

