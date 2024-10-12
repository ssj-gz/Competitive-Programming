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

int64_t metaDataSum(Node* subtreeRoot)
{
    int64_t sum = 0;
    for (const auto& metaData : subtreeRoot->metadataEntries)
        sum += metaData;
    for (auto* child : subtreeRoot->children)
        sum += metaDataSum(child);
    return sum;
}

int main()
{
    vector<int> licenseFile;
    int licenseFileEntry;
    while (cin >> licenseFileEntry)
        licenseFile.push_back(licenseFileEntry);
    int parserPos = 0;
    Node* rootNode = parseSubTree(licenseFile, parserPos);
    std::cout << "metadata sum: " << metaDataSum(rootNode) << std::endl;
    return 0;
}
