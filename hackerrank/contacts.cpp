// Simon St James (ssjgz) - 2019-04-09
#include <iostream>
#include <vector>
#include <memory>

#include <cassert>

using namespace std;

class SimpleTrie
{
    public:
        SimpleTrie()
        {
            m_rootNode = createNode();
        }
        void addString(const string& stringToAdd)
        {
            Node* currentNode = m_rootNode;
            for (const auto letter : stringToAdd)
            {
                const int letterIndex = letter - 'a';
                if (currentNode->nodeAfterLetterIndex[letterIndex] == nullptr)
                {
                    auto newNode = createNode();
                    currentNode->nodeAfterLetterIndex[letterIndex] = newNode;
                }

                currentNode = currentNode->nodeAfterLetterIndex[letterIndex];
                currentNode->numOccurrences++;
            }
        }
        int countStringsWithPrefix(const string& stringPrefixToFind) const
        {
            Node* currentNode = m_rootNode;
            for (const auto letter : stringPrefixToFind)
            {
                assert(currentNode != nullptr);
                const int letterIndex = letter - 'a';
                assert(letterIndex >= 0 && letterIndex < numLetters);
                if (currentNode->nodeAfterLetterIndex[letterIndex] == nullptr)
                {
                    return 0;
                }

                currentNode = currentNode->nodeAfterLetterIndex[letterIndex];
            }
            assert(currentNode != nullptr);
            return currentNode->numOccurrences;
        }
    private:
        static constexpr int numLetters = 26;
        struct Node
        {
            Node* nodeAfterLetterIndex[numLetters] = {};
            int numOccurrences = 0;
        };

        Node* m_rootNode = nullptr;
        vector<unique_ptr<Node>> m_nodes;

        Node* createNode()
        {
            m_nodes.push_back(make_unique<Node>());
            return m_nodes.back().get();
        }
};

int main()
{
    int n;
    cin >> n;

    SimpleTrie contactsTrie;

    for (int i = 0; i < n; i++)
    {
        string command;
        cin >> command;

        if (command == "add")
        {
            string nameToAdd;
            cin >> nameToAdd;

            contactsTrie.addString(nameToAdd);
        }
        else if (command == "find")
        {
            string namePrefixToFind;
            cin >> namePrefixToFind;

            const auto numFound = contactsTrie.countStringsWithPrefix(namePrefixToFind);
            cout << numFound << endl;
        }
    }
}
