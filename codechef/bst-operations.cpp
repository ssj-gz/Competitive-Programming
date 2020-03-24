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
    Node(Node* parent, int value, uint32_t position)
        : parent{parent}, value{value}, position{position}
    {
        static int dbgNextNodeId = 1;
        dbgId = dbgNextNodeId;
        dbgNextNodeId++;
    }

    Node* parent = nullptr;
    Node* leftChild;
    Node* rightChild;

    int value = -1;
    uint32_t position = 1;


    int dbgId = -1;
};

Node* minValueNode(Node* subtreeRoot)
{
    assert(subtreeRoot);
    if (!subtreeRoot->leftChild && !subtreeRoot->rightChild)
        return subtreeRoot;

    Node* minimum = nullptr;
    if (subtreeRoot->leftChild)
        minimum = minValueNode(subtreeRoot->leftChild);
    if (subtreeRoot->rightChild)
    {
        auto minOnRight = minValueNode(subtreeRoot->rightChild);
        if (!minimum || minOnRight < minimum)
            minimum = minOnRight;
    }
    assert(minimum);
    return minimum;
};

Node* deleteNodeWithValue(int value, Node* subtreeRoot)
{
    assert(subtreeRoot);
    if (subtreeRoot->value < value)
    {
        subtreeRoot->rightChild = deleteNodeWithValue(value, subtreeRoot->rightChild);
        return subtreeRoot;
    }
    else if (subtreeRoot->value > value)
    {
        subtreeRoot->leftChild = deleteNodeWithValue(value, subtreeRoot->leftChild);
        return subtreeRoot;
    }
    else
    {
        cout << subtreeRoot->position << endl;

        if (!subtreeRoot->leftChild)
            return subtreeRoot->rightChild;
        if (!subtreeRoot->rightChild)
            return subtreeRoot->leftChild;


        // Node to remove has two children; how do we re-structure the tree?
        // Probably lots of ways of doing this; I'll use the logic in 
        //
        //   https://www.geeksforgeeks.org/binary-search-tree-set-2-delete/
        //
        auto descendantWithMinValue = minValueNode(subtreeRoot->rightChild);
        assert(!descendantWithMinValue->leftChild);
        descendantWithMinValue->leftChild = subtreeRoot->leftChild;
        subtreeRoot->leftChild->parent = descendantWithMinValue;
        descendantWithMinValue->parent->leftChild = descendantWithMinValue->rightChild;
        descendantWithMinValue->rightChild = subtreeRoot->rightChild;
        subtreeRoot->rightChild->parent = descendantWithMinValue;
        descendantWithMinValue->parent = subtreeRoot->parent;

        return descendantWithMinValue;

    }
}

void printSubTree(Node* subtreeRoot)
{
    if (!subtreeRoot)
        return;

    cout << "Node with id: " << subtreeRoot->dbgId << " value: " << subtreeRoot->value << " position: " << subtreeRoot->position << " leftChild: " << (subtreeRoot->leftChild ? subtreeRoot->leftChild->dbgId : -1) <<" rightChild: " << (subtreeRoot->rightChild ? subtreeRoot->rightChild->dbgId : -1) << endl;
    printSubTree(subtreeRoot->leftChild);
    printSubTree(subtreeRoot->rightChild);
};

bool isBst(Node* subtreeRoot, int minValue, int maxValue)
{
    if (!subtreeRoot)
        return true;
    return isBst(subtreeRoot->leftChild, minValue, subtreeRoot->value) &&
           isBst(subtreeRoot->rightChild, subtreeRoot->value, maxValue);
}

bool isBst(Node* root)
{
    return isBst(root, std::numeric_limits<int>::max(), std::numeric_limits<int>::min());
}


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

    Node* rootNode = nullptr;

    for (int i = 0; i < numQueries; i++)
    {
        const auto queryType = read<char>();
        if (queryType == 'i')
        {
            const auto valueToInsert = read<int>();
            Node* newNode = nullptr;

            if (!rootNode)
            {
                rootNode = new Node(nullptr, valueToInsert, 1);
                newNode = rootNode;
            }
            else
            {
                Node* currentNode = rootNode;
                while (true)
                {
                    if (currentNode->value > valueToInsert)
                    {
                        if (currentNode->leftChild)
                            currentNode = currentNode->leftChild;
                        else
                        {
                            currentNode->leftChild = new Node(currentNode, valueToInsert, 2 * currentNode->position);
                            newNode = currentNode->leftChild;
                            break;
                        }
                    }
                    else if (currentNode->value < valueToInsert)
                    {
                        if (currentNode->rightChild)
                            currentNode = currentNode->rightChild;
                        else
                        {
                            currentNode->rightChild = new Node(currentNode, valueToInsert, 2 * currentNode->position + 1);
                            newNode = currentNode->rightChild;
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
            rootNode = deleteNodeWithValue(valueToDelete, rootNode);
        }

        cout << "After query: " << queryType << " tree: " << endl;
        printSubTree(rootNode);
        assert(isBst(rootNode));
    }

    assert(cin);
}
