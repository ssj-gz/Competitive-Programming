#include <cmath>
#include <cstdio>
#include <vector>
#include <list>
#include <iostream>
#include <algorithm>
using namespace std;

struct Node
{
    Node(int value)
        : value(value)
    {

    }
    long value;
    list<Node*> neighbours;
};

long smallestTreeDiff = numeric_limits<long>::max();

int calcDescendantsValueAndCutPoint(Node* node, Node* parentNode, long totalValue)
{
    int value = node->value;
    for (list<Node*>::iterator childIter = node->neighbours.begin(); childIter != node->neighbours.end(); childIter++)
    {
        Node *child = *childIter;
        if (child == parentNode) // The edges we are given don't give a parent-child relationship, so be careful not to traverse back up to parent!
            continue;

        const int childDescendantsValue = calcDescendantsValueAndCutPoint(child, node, totalValue);
        value += childDescendantsValue;

        const int treeDiff = abs((totalValue - childDescendantsValue) - childDescendantsValue);
        if (treeDiff < smallestTreeDiff)
        {
            smallestTreeDiff = treeDiff;
        }
    }
    return value;
}

int main() {
    /* Enter your code here. Read input from STDIN. Print output to STDOUT */   
    int N;
    cin >> N;
    vector<Node*> allNodes;
    long totalValue = 0;
    for (int i = 0; i < N; i++)
    {
        long value = 0;
        cin >> value;
        allNodes.push_back(new Node(value));
        totalValue += value;
    }
    vector<pair<int, int> > edges;
    for (int i = 0; i < N - 1; i++)
    {
        int node1Id, node2Id;
        cin >> node1Id >> node2Id;
        allNodes[node1Id - 1]->neighbours.push_back(allNodes[node2Id - 1]);
        allNodes[node2Id - 1]->neighbours.push_back(allNodes[node1Id - 1]);
    }
    Node *rootNode = allNodes[0];
    calcDescendantsValueAndCutPoint(rootNode, NULL, totalValue);
    cout << smallestTreeDiff;

    return 0;
}

