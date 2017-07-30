#include <vector>
#include <list>
#include <string>
#include <iostream>
#include <cassert>

using namespace std;

class TrieThing
{
    public:
        TrieThing()
        {
            m_nodes.push_back(Node());
            m_nodes.reserve(4'000'000);

        }
        class NodeRef
        {
            public:
                NodeRef(const NodeRef& other)
                    : m_nodeIndex(other.m_nodeIndex),
                    m_posInUnsplitString(other.m_posInUnsplitString)
                {

                }
                NodeRef()
                    : m_nodeIndex(-1),
                    m_posInUnsplitString(-1)
                {
                }
            private:
                int m_nodeIndex;
                // Only used for unsplit nodes.
                int m_posInUnsplitString;

                NodeRef(int nodeIndex, int posInUnsplitString)
                    : m_nodeIndex(nodeIndex),
                    m_posInUnsplitString(posInUnsplitString)
                {
                }
                friend class TrieThing;

                int nodeIndex() const
                {
                    return m_nodeIndex;
                }
                int posInUnsplitString() const
                {
                    return m_posInUnsplitString;
                }
        };
        NodeRef addString(const string& stringToAdd)
        {
            return addString(stringToAdd, 0, stringToAdd.size());
        }
        NodeRef addString(const string& stringToAdd, string::size_type pos, string::size_type count)
        {
            NodeRef startNodeRef(0, m_nodes[0].isSplit ? -1 : 0);
            return addSuffix(startNodeRef, stringToAdd, pos, count);
        }
        NodeRef addSuffix(NodeRef& prefixNodeRef, const string& suffix)
        {
            return addSuffix(prefixNodeRef, suffix, 0, suffix.size());
        }
        NodeRef addSuffix(NodeRef& prefixNodeRef, const string& suffix, string::size_type pos, string::size_type count)
        {
            int nodeIndex = prefixNodeRef.nodeIndex();
            int posInUnsplitString = prefixNodeRef.posInUnsplitString();
            Node* node = &(m_nodes[nodeIndex]);
            auto createNewNode = [&m_nodes = this->m_nodes]()
            {
                const bool exceededCapacity = m_nodes.size() == m_nodes.capacity();
                if (exceededCapacity)
                {
                    assert(false && "Exceeding vector capacity not supported!");
                }
                m_nodes.emplace_back();
                Node *newNode = &(m_nodes.back());
                return newNode;
            };
            while (count > 0)
            {
                if (node->isSplit)
                {
                    int nextNodeIndex = -1;
                    posInUnsplitString = -1;
                    const auto letter = suffix[pos];
                    const int letterIndex = letter - 'a';
                    assert(letterIndex >= 0 && letterIndex < 26);
                    for (const auto& x : node->nodeIdAfterLetterIndex)
                    {
                        if (x.first == letterIndex)
                        {
                            nextNodeIndex = x.second;
                            break;
                        }
                    }
                    if (nextNodeIndex == -1)
                    {
                        // Create new unsplit node with the remainder of the string.
                        Node *newNode = createNewNode();
                        newNode->unsplitString = suffix.substr(pos + 1, count);
                        nextNodeIndex = (m_nodes.size() - 1);
                        node->nodeIdAfterLetterIndex.push_back(make_pair(letterIndex, nextNodeIndex));
                        nodeIndex = nextNodeIndex;
                        posInUnsplitString = count - 1;
                        break;
                    }
                    else
                    {
                        pos++;
                        count--;
                        node = &(m_nodes[nextNodeIndex]);
                        nodeIndex = nextNodeIndex;
                        continue;
                    }

                }
                else
                {
                    if (posInUnsplitString == -1)
                        posInUnsplitString = 0;
                    if (posInUnsplitString == node->unsplitString.size())
                    {
                        if (node->nextNodeIndex == -1)
                        {
                            // Append the remaining to the end: we're done.
                            node->unsplitString += suffix.substr(pos, count);
                            posInUnsplitString = node->unsplitString.size();
                            break;
                        }
                        else
                        {
                            const int nextNodeIndex = node->nextNodeIndex;
                            node = &(m_nodes[nextNodeIndex]);
                            nodeIndex = nextNodeIndex;
                            posInUnsplitString = 0;
                            continue;
                        }
                    }
                    else
                    {
                        const size_t remainingInUnsplitString = node->unsplitString.size() > posInUnsplitString ? node->unsplitString.size() - posInUnsplitString : 0;
                        const int maxComparableLength = min(remainingInUnsplitString, count);
                        const char* nodeString = &(node->unsplitString[posInUnsplitString]);
                        const char* suffixToAdd = &(suffix[pos]);
                        int differentIndex = -1;
                        for (int i = 0; i < maxComparableLength; i++)
                        {
                            if (nodeString[i] != suffixToAdd[i])
                            {
                                differentIndex = i;
                                break;
                            }
                        }
                        if (differentIndex == -1)
                        {
                            posInUnsplitString += maxComparableLength;
                            count -= maxComparableLength;
                            pos += maxComparableLength;
                            continue;
                        }
                        else
                        {
                            // Strings are different - need to create new node.
                            assert(node->unsplitString.size() > 0);

                            Node *splitterNode = nullptr;
                            int splitterNodeId = -1;
                            if (posInUnsplitString + differentIndex == 0)
                            {
                                // Current node is the split node.
                                splitterNode = node;
                                splitterNodeId = nodeIndex;
                            }
                            else
                            {
                                // Create new splitter node.
                                splitterNode = createNewNode();
                                splitterNodeId = (m_nodes.size() - 1);
                            }
                            splitterNode->isSplit = true;

                            // Break out a node representing the old string and splice it into place.
                            Node* continuationNode = createNewNode();
                            const int continuationNodeId = (m_nodes.size() - 1);
                            continuationNode->unsplitString = (posInUnsplitString + differentIndex + 1) < node->unsplitString.size() ? node->unsplitString.substr(posInUnsplitString + differentIndex + 1) :"";
                            continuationNode->nextNodeIndex = node->nextNodeIndex;

                            // Add the remainder of the new suffix to a new, unsplit node.
                            Node* newUnSplitNode = createNewNode();
                            const int newUnSplitNodeId = (m_nodes.size() - 1);
                            newUnSplitNode->unsplitString = suffix.substr(pos + differentIndex + 1, count - differentIndex - 1);

                            node->nextNodeIndex = splitterNodeId;

                            assert(differentIndex < node->unsplitString.size());
                            assert(pos + differentIndex < suffix.size());
                            splitterNode->nodeIdAfterLetterIndex.push_back(make_pair(node->unsplitString[posInUnsplitString + differentIndex] - 'a', continuationNodeId));
                            splitterNode->nodeIdAfterLetterIndex.push_back(make_pair(suffix[pos + differentIndex] - 'a', newUnSplitNodeId));
                            splitterNode->nextNodeIndex = -1;

                            node->unsplitString = node->unsplitString.substr(0, posInUnsplitString + differentIndex);

                            if (prefixNodeRef.nodeIndex() == nodeIndex && prefixNodeRef.posInUnsplitString() >= posInUnsplitString + differentIndex)
                            {
                                if (prefixNodeRef.posInUnsplitString() == posInUnsplitString + differentIndex)
                                {
                                    prefixNodeRef = NodeRef(splitterNodeId, -1);
                                }
                                else
                                {
                                    prefixNodeRef = NodeRef(continuationNodeId, prefixNodeRef.posInUnsplitString() - (posInUnsplitString + differentIndex + 1));
                                }
                            }
                            nodeIndex = newUnSplitNodeId;
                            posInUnsplitString = newUnSplitNode->unsplitString.size();
                            break;

                        }
                    }
                }
                assert(false);
            }
            if (!node->isSplit && posInUnsplitString == -1)
                posInUnsplitString = 0;
            return NodeRef(nodeIndex, posInUnsplitString);
        }

        list<string> dump()
        {
            Node* node = &(m_nodes[0]);
            list<string> stringList;
            dumpInternal(node, "", stringList);
            return stringList;
        }
    private:
        static const int numLetters = 26;
        struct Node
        {
            bool isSplit = false;
            // Used if and only if not isSplit.
            string unsplitString;
            int nextNodeIndex = -1;

            // Used if and only if isSplit.
            vector<pair<int, int>> nodeIdAfterLetterIndex;
        };
        vector<Node> m_nodes;
        void dumpInternal(Node* node, string currentString, list<string>& stringList)
        {
            if (node->isSplit)
            {
                if (node->nodeIdAfterLetterIndex.empty())
                {
                    stringList.push_back(currentString);
                    return;
                }
                for (const auto& x : node->nodeIdAfterLetterIndex)
                {
                    const auto letterIndex = x.first;
                    const auto nodeIdAfterLetter = x.second;
                    assert(nodeIdAfterLetter > 0);
                    dumpInternal(&(m_nodes[nodeIdAfterLetter]), currentString
                            + static_cast<char>(letterIndex + 'a')
                            , stringList);
                }
            }
            else
            {
                currentString += node->unsplitString;
                if (node->nextNodeIndex == -1)
                {
                    stringList.push_back(currentString);
                    return;
                }
                dumpInternal(&(m_nodes[node->nextNodeIndex]), currentString, stringList);
            }
        }
};

int main()
{
    TrieThing t;
    auto prefixCursor = t.addString("prefix");
    t.addSuffix(prefixCursor, "a");
    t.addSuffix(prefixCursor, "b");
    t.addString("prefixc");

    auto stringList = t.dump();
    for (const auto& s : stringList)
    {
        cout << s << endl;
    }
}
