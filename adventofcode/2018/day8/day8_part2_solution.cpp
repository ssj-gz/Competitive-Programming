#include <iostream>
#include <vector>

using namespace std;

struct Node
{
    vector<Node*> children;
    vector<int> metadataEntries;
};

Node* parseSubTree(const vector<int>& licenseFile, int& parserPos)
{
    Node* node = new Node;
    const int numChildren = licenseFile[parserPos++];
    const int numMetaDataEntries = licenseFile[parserPos++];
    for (int childIndex = 0; childIndex < numChildren; childIndex++)
    {
        node->children.push_back(parseSubTree(licenseFile, parserPos));
    }
    for (int metadataIndex = 0; metadataIndex < numMetaDataEntries; metadataIndex++)
    {
        node->metadataEntries.push_back(licenseFile[parserPos++]);
    }
    return node;
}

int64_t nodeValue(Node* node)
{
    int64_t value = 0;
    if (node->children.empty())
    {
        for (const auto& metaData : node->metadataEntries)
            value += metaData;
    }
    else
    {
        for (const auto& metaData : node->metadataEntries)
        {
            int childIndex = metaData - 1;
            if (childIndex < 0 || childIndex >= node->children.size())
                continue;
            value += nodeValue(node->children[childIndex]);
        }

    }
    return value;
}

int main()
{
    vector<int> licenseFile;
    int licenseFileEntry;
    while (cin >> licenseFileEntry)
        licenseFile.push_back(licenseFileEntry);
    int parserPos = 0;
    Node* rootNode = parseSubTree(licenseFile, parserPos);
    std::cout << "root node value: " << nodeValue(rootNode) << std::endl;
    return 0;
}
