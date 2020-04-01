// Simon St James (ssjgz) - 2020-04-01
// 
// Solution to: https://www.codechef.com/problems/MVCN2TST
//
//#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#else
#define _GLIBCXX_DEBUG       // Iterator safety; out-of-bounds access for Containers, etc.
#pragma GCC optimize "trapv" // abort() on (signed) integer overflow.
#define BRUTE_FORCE
#endif
#include <iostream>
#include <vector>
#include <algorithm>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

struct Node
{
    vector<Node*> children;
    Node* parent = nullptr;
    int height = -1;
    int id = -1;

    int dfsBeginVisit = -1;
    int dfsEndVisit = -1;
};

void fillInDFSVisit(Node* node, int& dfsVisitNum)
{
    node->dfsBeginVisit = dfsVisitNum;
    dfsVisitNum++;

    for (auto child : node->children)
    {
        fillInDFSVisit(child, dfsVisitNum);
    }

    node->dfsEndVisit = dfsVisitNum;
    dfsVisitNum++;
}

void fillInDFSVisit(Node* rootNode)
{
    int dfsVisitNum = 1;
    fillInDFSVisit(rootNode, dfsVisitNum);
}

// Calculate the height of each node, and remove its parent from its list of "children".
void fixParentChildAndHeights(Node* node, Node* parent = nullptr, int height = 0)
{
    node->height = height;
    node->parent = parent;

    node->children.erase(remove(node->children.begin(), node->children.end(), parent), node->children.end());

    for (auto child : node->children)
    {
        fixParentChildAndHeights(child, node, height + 1);
    }
}

vector<pair<Node*, Node*>> computeValidReparentings(vector<Node>& nodes)
{
    vector<pair<Node*, Node*>> validReparentings;

    for (auto& nodeToReparent : nodes)
    {
        if (nodeToReparent.parent == nullptr)
            continue;

        for (auto& newParent : nodes)
        {
            assert(newParent.dfsBeginVisit != -1 && newParent.dfsEndVisit != -1);
            const bool newParentIsDescendant = (newParent.dfsBeginVisit >= nodeToReparent.dfsBeginVisit && newParent.dfsEndVisit <= nodeToReparent.dfsEndVisit);
            if (!newParentIsDescendant)
                validReparentings.push_back({&nodeToReparent, &newParent});
        }
    }

    return validReparentings;
}

vector<pair<Node*, Node*>> solveBruteForce(vector<Node>& nodes, const vector<int64_t>& queries)
{
    vector<pair<Node*, Node*>> result;
    auto rootNode = &(nodes.front());
    fillInDFSVisit(rootNode);

    auto validReparentings = computeValidReparentings(nodes);

    for (const auto query : queries)
    {
        const auto index = query - 1; // Make 0-relative.
        assert(0 <= index && index < validReparentings.size());
        auto queryResultIter = validReparentings.begin() + index;
        result.push_back(*queryResultIter);

        validReparentings.erase(queryResultIter);
    }

    return result;
}

#if 0
vector<pair<Node*, Node*>> solveOptimised()
{
    vector<pair<Node*, Node*>> result;
    
    return result;
}
#endif


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        // TODO - generate randomised test.
        //const int T = rand() % 100 + 1;
        const int T = 1;
        cout << T << endl;

        for (int t = 0; t < T; t++)
        {
            const auto numNodes = 1 + rand() % 10;
            vector<Node> nodes(numNodes);
            for (int i = 0; i < numNodes; i++)
                nodes[i].id = (i + 1);

            vector<pair<Node*, Node*>> edges;

            for (int childNodeIndex = 1; childNodeIndex < numNodes; childNodeIndex++)
            {
                const int parentIndex = rand() % childNodeIndex;
                edges.push_back({&(nodes[parentIndex]), &(nodes[childNodeIndex])});
            }
            for (auto& edge : edges)
            {
                edge.first->children.push_back(edge.second);
                edge.second->children.push_back(edge.first);
            }

            auto rootNode = &(nodes.front());
            fixParentChildAndHeights(rootNode);
            fillInDFSVisit(rootNode);
            const auto validReparentings = computeValidReparentings(nodes);

            const auto numQueries = validReparentings.empty() ? 0 : 1 + rand() % validReparentings.size();

            vector<int64_t> queries;
            for (int queryNum = 0; queryNum < numQueries; queryNum++)
            {
                queries.push_back(1 + rand() % (validReparentings.size() - queryNum));
            }

            cout << numNodes << endl;
            for (const auto edge : edges)
            {
                cout << edge.first->id << " " << edge.second->id << endl;
            }
            cout << numQueries << endl;
            for (const auto query : queries)
            {
                cout << query << endl;
            }
        }

        return 0;
    }
    
    // TODO - read in testcase.
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const auto numNodes = read<int>();

        vector<Node> nodes(numNodes);
        for (int i = 0; i < numNodes; i++)
        {
            nodes[i].id = i + 1;
        }
        for (auto i = 0; i < numNodes - 1; i++)
        {
            // Make a and b 0-relative.
            const auto a = read<int>() - 1;
            const auto b = read<int>() - 1;

            nodes[a].children.push_back(&nodes[b]);
            nodes[b].children.push_back(&nodes[a]);
        }

        auto rootNode = &(nodes.front());
        fixParentChildAndHeights(rootNode);

        // TODO - eventually, we will need to make this online, using a decryption-key
        // approach (similar to "Simple Markdown Editor").
        const auto numQueries = read<int>();
        vector<int64_t> queries(numQueries);
        for (auto& query : queries)
        {
            query = read<int64_t>();
        }


#ifdef BRUTE_FORCE
#if 1
        const auto solutionBruteForce = solveBruteForce(nodes, queries);
        cout << "solutionBruteForce: " << endl;
        for (const auto result : solutionBruteForce)
        {
            cout << "solutionBruteForce: " << result.first->id << " " << result.second->id << endl;
        }
#endif
#if 0
        const auto solutionOptimised = solveOptimised();
        cout << "solutionOptimised:  " << solutionOptimised << endl;

        assert(solutionOptimised == solutionBruteForce);
#endif
#else
        const auto solutionOptimised = solveOptimised();
        cout << solutionOptimised << endl;
#endif
    }

    assert(cin);
}
