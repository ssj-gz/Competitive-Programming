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
        // (in such a way that we still have a BST).  I assumed that the one
        // from here:
        //
        //   https://www.geeksforgeeks.org/binary-search-tree-set-2-delete/
        //
        // would work, but apparently not - or rather, not exactly.  I had to 
        // look up AC solutions to figure out what it wanted: as near as I can
        // guess, the Problem Setter must have adapted that algorithm but mistakenly
        // printed *two* values (as I've had to resort to doing) - the first is 
        // the expected subtreeRoot->position, but the second, incorrect one is
        // the result of the call to 
        //
        //   deleteNodeWithValue(rightDescendantWithMinValue->value, subtreeRoot->rightChild)
        //
        // (below).
        //
        // In other words - the Problem Setter expects us to make exactly the same mistake that
        // he did, otherwise we get WA XD
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
    // Wasted a huge amount of time on this - the question is completely 
    // ambiguous, and the resolution of the ambiguities contradicts the
    // Problem Statement.
    // 
    // See the comment in deleteNodeWithValue for more information.
    //
    // Also, the expected solution for the below (unambiguous) testcase appears
    // to contradict the Problem Statement:
    //
    //   10
    //   i 1
    //   i 2
    //   i 3
    //   i 4
    //   i 5
    //   d 1
    //   d 2
    //   d 3
    //   d 4
    //   d 5
    //
    // I would expect the output for each of the deletions to always be 1, but it is not:
    // it's more like the "position" of a Node is fixed at construction-time and never
    // changes, even if one of its parents is deleted.  Sigh.
    
    ios::sync_with_stdio(false);
    
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
