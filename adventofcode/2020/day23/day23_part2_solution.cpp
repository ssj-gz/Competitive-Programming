#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <cassert>

using namespace std;

int main()
{
    string input;
    cin >> input;

    struct Node
    {
        Node(int cupValue)
            : cupValue{cupValue}
        {
        }
        int cupValue = -1;

        // "next" and "prev", *in clockwise order*.
        Node* next = nullptr;
        Node* prev = nullptr;
    };

    // Read in initial cup values, and create the synthetic ones.
    constexpr int numCups = 1'000'000;
    constexpr int largestCupValue = numCups;
    vector<Node> cupNodes;
    for (const auto inputDigit : input)
        cupNodes.push_back(inputDigit - '0');
    int nextCupNum = static_cast<int>(cupNodes.size()) + 1;
    while (cupNodes.size() < numCups)
    {
        cupNodes.push_back(nextCupNum);
        nextCupNum++;
    }

    // Set up the nodeForCupValue lookup, and set up "next" and 
    // "prev" for cupNodes's, so that they form a ring.
    vector<Node*> nodeForCupValue(largestCupValue + 1);
    for (int i = 0; i < numCups; i++)
    {
        nodeForCupValue[cupNodes[i].cupValue] = &(cupNodes[i]);

        if (i >= 1)
            cupNodes[i].prev = &(cupNodes[i - 1]);
        if (i + 1 < numCups)
            cupNodes[i].next = &(cupNodes[i + 1]);
    }
    cupNodes.back().next = &(cupNodes.front());
    cupNodes.front().prev = &(cupNodes.back());

    // Begin working through the 10'000'000 iterations!
    int currentCup = cupNodes.front().cupValue;
    int moveNum = 0;
    while (true)
    {
        moveNum++;

        // Choose the three cups to remove.
        array<Node*, 3> threeCups{};
        Node* cupToRemoveNode = nodeForCupValue[currentCup]->next;
        assert(cupToRemoveNode->prev->cupValue == currentCup);
        for (auto& threeCupNode : threeCups)
        {
            threeCupNode = cupToRemoveNode;
            cupToRemoveNode = cupToRemoveNode->next;
        }

        // Remove the three cups.
        threeCups[0]->prev->next = threeCups[2]->next;
        threeCups[2]->next->prev = threeCups[0]->prev;

        // Compute the destination node ...
        int destination = currentCup - 1;
        while (true)
        {
            if (destination == 0)
                destination = largestCupValue;
            if (find_if(threeCups.begin(), threeCups.end(), [&destination](const auto node) { return node->cupValue == destination;}) == threeCups.end())
                break;
            destination--;
        }
        auto destinationNode = nodeForCupValue[destination];
        // ... and re-insert the three cups after it.
        auto originaNodeAfterDestination = destinationNode->next;
        destinationNode->next = threeCups[0];
        originaNodeAfterDestination->prev = threeCups[2];
        threeCups[0]->prev = destinationNode;
        threeCups[2]->next = originaNodeAfterDestination;

        if (moveNum == 10'000'000)
        {
            // All done.
            break;
        }

        // Prepare for next iteration.
        currentCup = nodeForCupValue[currentCup]->next->cupValue;
    }

    const auto cup1Node = nodeForCupValue[1];
    const auto firstNumberNode = cup1Node->next;
    const auto secondNumberNode = firstNumberNode->next;
    cout << "Final: " << static_cast<int64_t>(firstNumberNode->cupValue) * secondNumberNode->cupValue << endl;
}
