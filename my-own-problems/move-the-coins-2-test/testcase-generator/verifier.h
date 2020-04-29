#ifndef VERIFIER_H
#define VERIFIER_H

#include <vector>

using namespace std;

namespace Verifier
{
    struct Node
    {
        vector<Node*> children;
        Node* parent = nullptr;
        int height = -1;
        int id = -1;
        int numDescendants = -1; // Includes the node itself.

        bool isDescendantOf(Node& otherNode)
        {
            return (dfsBeginVisit >= otherNode.dfsBeginVisit && dfsEndVisit <= otherNode.dfsEndVisit);
        }

        int dfsBeginVisit = -1;
        int dfsEndVisit = -1;
    };


    int64_t calcFinalDecryptionKey(vector<Node>& nodes, const vector<int64_t>& encryptedQueries, vector<int64_t>& destDecryptedQueries);
}


#endif

