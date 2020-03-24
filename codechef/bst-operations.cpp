// Simon St James (ssjgz) - 2020-03-24
// 
// Solution to: https://www.codechef.com/problems/BSTOPS
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
#include <memory>

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

#if 0
SolutionType solveBruteForce()
{
    SolutionType result;
    
    return result;
}
#endif

#if 0
SolutionType solveOptimised()
{
    SolutionType result;
    
    return result;
}
#endif

struct Node
{
    Node()
    {
        static int dbgNextNodeId = 1;
        dbgId = dbgNextNodeId;
        dbgNextNodeId++;
    }

    int value = -1;
    uint32_t position = 1;

    unique_ptr<Node> leftChild;
    unique_ptr<Node> rightChild;

    int dbgId = -1;
};

void printSubTree(Node* subtreeRoot)
{
    if (!subtreeRoot)
        return;

    cout << "Node with id: " << subtreeRoot->dbgId << " value: " << subtreeRoot->value << " position: " << subtreeRoot->position << " leftChild: " << (subtreeRoot->leftChild ? subtreeRoot->leftChild->dbgId : -1) <<" rightChild: " << (subtreeRoot->rightChild ? subtreeRoot->rightChild->dbgId : -1) << endl;
    printSubTree(subtreeRoot->leftChild.get());
    printSubTree(subtreeRoot->rightChild.get());
};


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
        }

        return 0;
    }
    
    const int numQueries = read<int>();

    unique_ptr<Node> rootNode = nullptr;

    for (int i = 0; i < numQueries; i++)
    {
        const auto queryType = read<char>();
        if (queryType == 'i')
        {
            const auto valueToInsert = read<int>();
            Node* newNode = nullptr;

            if (!rootNode)
            {
                rootNode = make_unique<Node>();
                rootNode->position = 1;
                rootNode->value = valueToInsert;
                newNode = rootNode.get();
            }
            else
            {
                Node* currentNode = rootNode.get();
                while (true)
                {
                    if (currentNode->value > valueToInsert)
                    {
                        if (currentNode->leftChild)
                            currentNode = currentNode->leftChild.get();
                        else
                        {
                            currentNode->leftChild = make_unique<Node>();
                            currentNode->leftChild->value = valueToInsert;
                            currentNode->leftChild->position = 2 * currentNode->position;
                            newNode = currentNode->leftChild.get();
                            break;
                        }
                    }
                    else if (currentNode->value < valueToInsert)
                    {
                        if (currentNode->rightChild)
                            currentNode = currentNode->rightChild.get();
                        else
                        {
                            currentNode->rightChild = make_unique<Node>();
                            currentNode->rightChild->value = valueToInsert;
                            currentNode->rightChild->position = 2 * currentNode->position + 1;
                            newNode = currentNode->rightChild.get();
                            break;
                        }
                    }
                    else
                    {
                        assert(false && "There are not supposed to be any duplicates!");
                    }
                }
            }
            cout << newNode->position << endl;

        }
        else if (queryType == 'd')
        {
            const auto valueToDelete = read<int>();

            Node* parent = nullptr;
            Node* currentNode = rootNode.get();
            uint32_t deletedNodePos = 0;
            while (true)
            {
                assert(currentNode);
                if (currentNode->value == valueToDelete)
                {
                    deletedNodePos = currentNode->position;
                    if (parent == nullptr)
                    {
                        rootNode.reset();
                    }
                    else
                    {
                        if (parent->leftChild.get() == currentNode)
                            parent->leftChild.reset();
                        if (parent->rightChild.get() == currentNode)
                            parent->rightChild.reset();
                    }
                    break;
                }
                parent = currentNode;
                if (currentNode->value < valueToDelete)
                    currentNode = currentNode->rightChild.get();
                if (currentNode->value > valueToDelete)
                    currentNode = currentNode->leftChild.get();

            }
            cout << deletedNodePos << endl;
        }

        //cout << "After query: " << queryType << " tree: " << endl;
        //printSubTree(rootNode.get());
    }

    assert(cin);
}
