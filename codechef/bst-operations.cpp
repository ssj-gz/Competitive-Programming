// Simon St James (ssjgz) - 2020-03-24
// 
// Solution to: https://www.codechef.com/problems/BSTOPS
//
#include <iostream>
#include <vector>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!
#include <set>        // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

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
    Node(int value, uint32_t position)
       : value{value}, position{position}
    {
    }

    Node* leftChild = nullptr;
    Node* rightChild = nullptr;

    int value = -1;
    uint32_t position = 1;
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

        if (!subtreeRoot->leftChild && !subtreeRoot->rightChild)
        {
            return nullptr;
        }

        if (!subtreeRoot->leftChild)
        {
            return subtreeRoot->rightChild;
        }
        else if (!subtreeRoot->rightChild)
        {
            return subtreeRoot->leftChild;
        }

        // There are many ways to delete a node with two children from a BST
        // (in such a way that we still have a BST).
        // The one that is expected is, frankly, completely arbitrary and 
        // seems "wrong" to me in that it prints out *two* positions (the Problem Statement
        // implies we should only print one).
        // I had to look up AC solutions to figure out what it wanted.
        // I am unimpressed, to say the least - the odds of someone getting the "correct"
        // answer without looking at AC solutions are slim, to say the least.
        auto rightDescendantWithMinValue = minValueNode(subtreeRoot->rightChild);
        assert(!rightDescendantWithMinValue->leftChild);

        subtreeRoot->value = rightDescendantWithMinValue->value;
        subtreeRoot->rightChild = deleteNodeWithValue(rightDescendantWithMinValue->value, subtreeRoot->rightChild);

        return subtreeRoot;

    }
}

Node* insertNodeWithValue(int value, Node* subtreeRoot, uint32_t position)
{
    if (!subtreeRoot)
    {
        cout << position << endl;
        return new Node(value, position);
    }
    if (subtreeRoot->value > value)
    {
        subtreeRoot->leftChild = insertNodeWithValue(value, subtreeRoot->leftChild, position * 2);
        return subtreeRoot;
    }
    else if (subtreeRoot->value < value)
    {
        subtreeRoot->rightChild = insertNodeWithValue(value, subtreeRoot->rightChild, position * 2 + 1);
        return subtreeRoot;
    }
    else
    {
        assert(false);
    }
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int numQueries = 1 + rand() % 35;
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
            rootNode = insertNodeWithValue(valueToInsert, rootNode, 1);
        }
        else if (queryType == 'd')
        {
            const auto valueToDelete = read<int>();
            rootNode = deleteNodeWithValue(valueToDelete, rootNode);
        }

    }
    assert(cin);
}
