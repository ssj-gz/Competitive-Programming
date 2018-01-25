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
    // Fairly tricky one.  We clearly have an impartial game, here, so it looks 
    // like finding the Grundy number for the game played on a given  tree is the way 
    // forward.
    //
    // Given a tree where the root R has m children C1, C2, ... Cm
    //
    //                      O(R)
    //                 /      |    ...   \
    //                O(C1)   O(C2) ...  OC(m)
    //                |      /  \      / | \
    //                O     O    O    O  O  O
    //               ...         |    .......
    //                           O
    //                          ...   
    //                      
    // It's hopefully obvious that this is equivalent to splitting R into m root nodes R1, R2, ... RM
    // and running the games in parallel on the resulting trees i.e. on the set of M trees
    //
    //    O(R1)         O(R2)    ...      O(RM)
    //     /            |                 \
    //    O(C1)         O(C2) ..          OC(m)
    //    |            /  \             / | \
    //    O           O    O           O  O  O
    //   ...               |           .......
    //                     O
    //                    ...   
    //
    // For a node v, let grundy(v) be the grundy number of the subtree rooted at v; thus, by the Sprague-Grundy
    // Theorem, if m > 1:
    //
    //  grundy(R) = grundy(R1) ^ grundy(R2) ^ ... ^ grundy(RM)
    //
    // This is some progress, but not much: we've "reduced" the problem from one with a single tree with N nodes to
    // one with a forest of trees with a total of N + (M - 1) nodes (M > 1)!
    //
    // Perhaps we can find grundy(Ri) in terms of grundy(Ci), which would give us a nice way of decomposing the problem
    // into smaller problems? In fact, yes we can: intuitively, grundy(Ci) represents the number of stones in a Nim
    // pile corresponding to the subtree rooted at Ci; thus, since Ri adds one edge ("stone") to this, we might assume
    // that
    //
    //  grundy(Ri) = 1 + grundy(C)
    //
    // This is indeed the case, but let's be a bit more rigorous about things!
    //
    // Theorem
    //
    // Let T be a tree of M nodes with root R which has a single child C.  Then 
    //
    //  grundy(R) = 1 + grundy(C).
    //
    // Proof
    //
    //      O(R)
    //      | e0
    //      O(C)
    //     /  \ 
    //    O    O 
    //         | 
    //         O
    //        ...
    //
    // Proof is by induction on M; assume that given any rooted tree with root R' with single child C' and total < M nodes
    // satisfies grundy(R') = 1 + grundy(C').  The base case M = 2 is obvious: the grundy number for R' is 1,
    // while the grundy number for the tree rooted at C' (which consists only of C' itself) is 0 (C' has no children -> no
    // moves are possible).
    //
    // Let e0 be the edge connecting R and C, and e1, e2, ... e_m-2 be the other edges of T (those in the subtree rooted at C).
    // For a given vertex v, let V - ei be the subtree resulting from making the Move consisting of removing the edge ei, as described in the game.
    //
    // By Sprague-Grundy,
    //
    //   grundy(R) = mex [ i = 0, 1, ... m - 2 ] { grundy(R - ei) }
    //              = mex (grundy(R - e0), mex [ i = 1, 2, ... m - 2] { grundy(R - ei) }
    //              = mex (0, mex [ i = 1, 2, ... m - 2] { grundy(R - ei) }    (1)
    //
    // since grundy(R - e0) is the subtree consisting of just the vertex R, which has grundy number 0.
    //
    // Note that for i > 0, removing the edge ei would be a valid Game Move for the subtree rooted at C.  Again, by the Sprague-Grundy
    // Theorem, we have:
    //
    //   grundy(C) = mex [ i = 1, 2, ... m - 2 ] { grundy(C - ei) }    (2)
    //
    // Also, since C - ei is a valid Game Move for the subtree rooted at C which results in a tree which is strictly smaller than T i.e.
    // which has strictly less than M nodes, we have, by induction hypothesis:
    //
    //   grundy(R - ei) = 1 + grundy(C - ei) for all i > 0.
    //
    // Plugging this into (1) gives:
    //
    //   grundy(R) = mex (0, mex [ i = 1, 2, ... m - 2] { 1 + grundy(C - ei) }   (3)
    //
    // It's easily shown that given any integers a1, a2, ... an, we have
    //
    //   mex(0, mex [ i = 1, 2, ... n ] { 1 + an }) = 1 + mex [ i = 1, 2, ... n ] {an}
    // 
    // (proof is left as an exercise for the reader!), so using this with (3), we have 
    //
    //   grundy(R) = 1 + mex [ i = 1, 2, ... m - 2] { grundy(C - ei) } 
    //
    // Substituting (2) into this gives:
    //
    //   grundy(R) = 1 + grundy(C) 
    //
    // as required.
    //
    // So, in sum, we have that, if T is a tree with root R which has M children C1, C2, ... CM, then:
    //
    //  if M == 1, then grundy(R) = 1 + grundy(C1)
    //  else if M > 1, grundy(R) = (1 + grundy(C1)) ^ (1 + grundy(C2)) ^ .... ^ (1 + grundy(CM))
    //  else if M == 0, grundy(R) = 0.
    //
    // This give us a neat way of recursively finding the grundy number for a given tree, which tells us who 
    // wins the game (the first player, Alice, wins if and only if the grundy(R) != 0).
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
