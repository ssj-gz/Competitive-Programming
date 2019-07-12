#include <iostream>
#include <vector>

#include <cassert>

#include <sys/time.h>

using namespace std;

struct Node
{
    vector<Node*> neighbours;

    bool hasPerson = false;
};

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int numNodes = 1 + rand() % 100;
        cout << numNodes << endl;

        for (int i = 0; i < numNodes - 1; i++)
        {
            cout << (i + 2) << " " << ((rand() % (i + 1) + 1)) << endl;
        }
        for (int i = 0; i < numNodes; i++)
        {
            cout << (rand() % 2) << endl;
        }
        return 0;
    }
    int numNodes;
    cin >> numNodes;

    vector<Node> nodes(numNodes);

    for (int i = 0; i < numNodes - 1; i++)
    {
        int u;
        cin >> u;
        int v;
        cin >> v;

        // Make 0-relative.
        u--;
        v--;

        nodes[u].neighbours.push_back(&(nodes[v]));
        nodes[v].neighbours.push_back(&(nodes[u]));
    }
    for (int i = 0; i < numNodes; i++)
    {
        int hasPerson;
        cin >> hasPerson;

        nodes[i].hasPerson = (hasPerson == 1);
    }
    assert(cin);
}
