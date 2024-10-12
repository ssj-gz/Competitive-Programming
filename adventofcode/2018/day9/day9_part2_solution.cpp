#define _GLIBCXX_DEBUG       // Iterator safety; out-of-bounds access for Containers, etc.
#include <iostream>
#include <vector>
#include <regex>

#include <cassert>

using namespace std;

struct Node
{
    Node(int value)
        : value{value}
    {
    }
    int value = -1;
    Node *clockwise = nullptr;
    Node *antiClockwise = nullptr;
};

int main()
{
    std::regex gameSpecRegex(R"(^(\d+) players; last marble is worth (\d+) points$)");
    string gameSpecLine;
    getline(cin, gameSpecLine);
    std::smatch gameSpecMatch;
    const bool matchSuccessful = std::regex_match(gameSpecLine, gameSpecMatch, gameSpecRegex);
    assert(matchSuccessful);
    const int numPlayers = std::stoi(gameSpecMatch[1]);
    const int highestMarble = std::stoi(gameSpecMatch[2]) * 100;
    std::cout << "numPlayers: " << numPlayers << " highestMarble: " << highestMarble << std::endl;
    int nextMarbleValue = 1;
    int nextPlayerIndex = 0;
    Node* currentMarbleNode = new Node(0);
    currentMarbleNode->clockwise = currentMarbleNode;
    currentMarbleNode->antiClockwise = currentMarbleNode;
    vector<int64_t> playerScores(numPlayers);
    while (nextMarbleValue <= highestMarble)
    {
        if (nextMarbleValue % 1000 == 0)
            std::cout << "nextMarbleValue: " << nextMarbleValue << std::endl;
        if (nextMarbleValue % 23 != 0)
        {
            //std::cout << "Not divisible by 23; adding" << std::endl;
            Node* insertBeforeNode = currentMarbleNode->clockwise->clockwise;
            Node* newNode = new Node(nextMarbleValue);
            auto* prevAnticlockwise = insertBeforeNode->antiClockwise;
            prevAnticlockwise->clockwise = newNode;
            newNode->antiClockwise = prevAnticlockwise;

            insertBeforeNode->antiClockwise = newNode;
            newNode->clockwise = insertBeforeNode;
            
            currentMarbleNode = newNode;
        }
        else
        {
            //std::cout << "Divisible by 23; removing" << std::endl;
            playerScores[nextPlayerIndex] += nextMarbleValue;
            Node *nodeToRemove = currentMarbleNode;
            for (int i = 0; i < 7; i++)
            {
                nodeToRemove = nodeToRemove->antiClockwise;
            }
            currentMarbleNode = nodeToRemove->clockwise;
            playerScores[nextPlayerIndex] += nodeToRemove->value;
            nodeToRemove->antiClockwise->clockwise = nodeToRemove->clockwise;
            nodeToRemove->clockwise->antiClockwise = nodeToRemove->antiClockwise;
            delete nodeToRemove;
        }
#if 0
        std::cout << "Marbles: "<<  std::endl;
        Node* node = currentMarbleNode;
        do
        {
            if (node == currentMarbleNode)
                std::cout << "(" << node->value << ") ";
            else
                std::cout << node->value << " ";
            assert(node->clockwise->antiClockwise == node);
            node = node->clockwise;
        } while (node != currentMarbleNode);

        std::cout << endl;
#endif

        nextMarbleValue++;
        nextPlayerIndex = (nextPlayerIndex + 1) % numPlayers;
    }
    std::cout << " highest score: " << *std::max_element(playerScores.begin(), playerScores.end()) << std::endl;
    return 0;
}
