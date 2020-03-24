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
#include <limits>
#include <set>

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
    Node(Node* parent, int value)
        : parent{parent}, value{value}
    {
        static int dbgNextNodeId = 1;
        dbgId = dbgNextNodeId;
        dbgNextNodeId++;
    }

    Node* parent = nullptr;
    Node* leftChild = nullptr;
    Node* rightChild = nullptr;

    int value = -1;
    //uint32_t position = 1;


    int dbgId = -1;
};

Node* minValueNode(Node* subtreeRoot)
{
    assert(subtreeRoot);
    if (!subtreeRoot->leftChild && !subtreeRoot->rightChild)
        return subtreeRoot;

    Node* minimum = subtreeRoot;
    if (subtreeRoot->leftChild)
    {
        auto minOnLeft = minValueNode(subtreeRoot->leftChild);
        if (minOnLeft->value < minimum->value)
            minimum = minOnLeft;
    }
    if (subtreeRoot->rightChild)
    {
        auto minOnRight = minValueNode(subtreeRoot->rightChild);
        if (minOnRight->value < minimum->value)
            minimum = minOnRight;
    }
    assert(minimum);
    return minimum;
};

Node* deleteNodeWithValue(int value, Node* subtreeRoot, uint32_t position)
{
    assert(subtreeRoot);
    if (subtreeRoot->value < value)
    {
        subtreeRoot->rightChild = deleteNodeWithValue(value, subtreeRoot->rightChild, position * 2 + 1);
        if (subtreeRoot->rightChild)
            subtreeRoot->rightChild->parent = subtreeRoot;
        return subtreeRoot;
    }
    else if (subtreeRoot->value > value)
    {
        subtreeRoot->leftChild = deleteNodeWithValue(value, subtreeRoot->leftChild, position * 2);
        if (subtreeRoot->leftChild)
            subtreeRoot->leftChild->parent = subtreeRoot;
        return subtreeRoot;
    }
    else
    {
        cout << position << endl;

        if (!subtreeRoot->leftChild && !subtreeRoot->rightChild)
        {
            return nullptr;
        }

        if (!subtreeRoot->leftChild)
        {
            return subtreeRoot->rightChild;
        }
        if (!subtreeRoot->rightChild)
        {
            return subtreeRoot->leftChild;
        }


        // Node to remove has two children; how do we re-structure the tree?
        // Probably lots of ways of doing this; I'll use the logic in 
        //
        //   https://www.geeksforgeeks.org/binary-search-tree-set-2-delete/
        //
        auto descendantWithMinValue = minValueNode(subtreeRoot->rightChild);
        //cout << "descendantWithMinValue id:" << descendantWithMinValue->dbgId << " value: " << descendantWithMinValue->value << endl;
        assert(!descendantWithMinValue->leftChild);

        descendantWithMinValue->leftChild = subtreeRoot->leftChild;
        descendantWithMinValue->leftChild->parent = descendantWithMinValue;

        if (descendantWithMinValue->parent != subtreeRoot)
        {
            descendantWithMinValue->parent->leftChild = descendantWithMinValue->rightChild;
            if (descendantWithMinValue->parent->leftChild)
                descendantWithMinValue->parent->leftChild->parent = descendantWithMinValue->parent;

            descendantWithMinValue->rightChild = subtreeRoot->rightChild;
            if (subtreeRoot->rightChild)
                subtreeRoot->rightChild->parent = descendantWithMinValue;
        }

        //descendantWithMinValue->parent->leftChild = nullptr;


        //descendantWithMinValue->parent = subtreeRoot->parent;

        delete subtreeRoot;

        return descendantWithMinValue;

    }
}

Node* insertNodeWithValue(int value, Node* subtreeRoot, Node* parent, uint32_t position)
{
    if (!subtreeRoot)
    {
        cout << position << endl;
        return new Node(parent, value);
    }
    if (subtreeRoot->value > value)
    {
        subtreeRoot->leftChild = insertNodeWithValue(value, subtreeRoot->leftChild, subtreeRoot, position * 2);
        return subtreeRoot;
    }
    else if (subtreeRoot->value < value)
    {
        subtreeRoot->rightChild = insertNodeWithValue(value, subtreeRoot->rightChild, subtreeRoot, position * 2 + 1);
        return subtreeRoot;
    }
    else
    {
        assert(false);
    }
}


void printSubTree(Node* subtreeRoot)
{
    if (!subtreeRoot)
        return;

    //cout << "Node with id: " << subtreeRoot->dbgId << " value: " << subtreeRoot->value << " position: " << subtreeRoot->position << " leftChild: " << (subtreeRoot->leftChild ? subtreeRoot->leftChild->dbgId : -1) <<" rightChild: " << (subtreeRoot->rightChild ? subtreeRoot->rightChild->dbgId : -1) << " parent: " <<  (subtreeRoot->parent ? subtreeRoot->parent->dbgId : -1) << endl;
    printSubTree(subtreeRoot->leftChild);
    printSubTree(subtreeRoot->rightChild);
};

bool isBst(Node* subtreeRoot, Node* parent, int minValue, int maxValue)
{
    if (!subtreeRoot)
        return true;
    assert(subtreeRoot->parent == parent);
    //cout << "minValue: " << minValue << " maxValue: " << maxValue << " value: " << subtreeRoot->value << endl;
    if (subtreeRoot->value < minValue)
        return false;
    if (subtreeRoot->value > maxValue)
        return false;
    return isBst(subtreeRoot->leftChild, subtreeRoot, minValue, subtreeRoot->value) &&
           isBst(subtreeRoot->rightChild, subtreeRoot, subtreeRoot->value, maxValue);
}

bool isBst(Node* root)
{
    return isBst(root, nullptr, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
}


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int numQueries = 1 + rand() % 1000;
        cout << numQueries << endl;
        set<int> valuesInTree;
        for (int i = 0; i < numQueries; i++)
        {
            const int queryType = rand() % (1 + (valuesInTree.empty() ? 0 : 1));
            if (queryType == 0)
            {
                int valueToInsert = 1;
                while (valuesInTree.find(valueToInsert) != valuesInTree.end())
                    valueToInsert++;
                cout << "i " << valueToInsert << endl;
                valuesInTree.insert(valueToInsert);
            }
            else
            {
                assert(!valuesInTree.empty());
                int valueToDeleteIndex = rand() % valuesInTree.size();
                auto valueToDeleteIter = valuesInTree.begin();
                while (valueToDeleteIndex > 0)
                {
                    valueToDeleteIndex--;
                    valueToDeleteIter++;
                    assert(valueToDeleteIter != valuesInTree.end());
                }
                assert(valueToDeleteIter != valuesInTree.end());
                cout << "d " << *valueToDeleteIter << endl;
                valuesInTree.erase(valueToDeleteIter);
            }
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
            //cout << "Insert value: " << valueToInsert << endl;
            uint32_t newNodePosition = 0;

            rootNode = insertNodeWithValue(valueToInsert, rootNode, nullptr, 1);

        }
        else if (queryType == 'd')
        {
            const auto valueToDelete = read<int>();
            //cout << "Delete value: " << valueToDelete << endl;
            rootNode = deleteNodeWithValue(valueToDelete, rootNode, 1);
            if (rootNode)
                rootNode->parent = nullptr;
        }

        //cout << "After query: " << queryType << " tree: " << endl;
        printSubTree(rootNode);
        assert(isBst(rootNode));
    }
    //cout << "done" << endl;

    assert(cin);
}
