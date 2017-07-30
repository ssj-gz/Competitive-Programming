#include <iostream>
#include <vector>
#include <array>
#include <cassert>

using namespace std;

const  long maxHeight = 100'000;
constexpr long log2Plus1(long n)
{
    return (n == 1) ? 1 : 1 + log2Plus1(n / 2);
}
constexpr long maxHeightLog2 = log2Plus1(maxHeight);

struct Node
{
    Node()
    {
    }
    Node(long id, long parentId)
        : id(id),
        parent(parentId),
        exists(true)
    {
    }
    long id = -1;
    long parent = -1;
    long height = -1;
    bool exists = false;
    // i.e. powerOf2HeightAncestors[m] is the id of the nearest ancestor Node whose height
    // is a multiple of (1 << m).
    array<long, maxHeightLog2> powerOf2HeightAncestors = {};
    vector<long> children;
};

vector<Node> allNodes;

long kthAncestor(const Node& node, long k);

void updateAncestorTrackingFromParent(Node& node, const Node& parentNode, const vector<long>& ancestors)
{
    node.parent = parentNode.id;
    if (node.height == -1)
    {
        node.height = parentNode.height + 1;
    }
    copy(parentNode.powerOf2HeightAncestors.begin(), parentNode.powerOf2HeightAncestors.end(), node.powerOf2HeightAncestors.begin());
    for (int m = 0; m < maxHeightLog2; m++)
    {
        if (node.height < (1 << m) || ((node.height - 1) % (1 << m)) != 0)
        {
            // This m hasn't changed from parent, so neither will subsequent m.
            break;
        }

        const long milestoneAncestorHeight = ((node.height - 1) / (1 << m)) * (1 << m);
        long milestoneAncestorId = -1;
        if (!ancestors.empty())
        {
            // Can lookup the ancestor easily :)
            assert(milestoneAncestorHeight < ancestors.size());
            milestoneAncestorId = ancestors[milestoneAncestorHeight];
            assert(milestoneAncestorId != node.id);
        }
        else
        {
            // Must compute the ancestor.  Since this node is not fully Tracked yet,
            // we must run kthAncestor on its *parent* (with adjusted milestoneAncestorDistance,
            // of course).
            const long milestoneAncestorDistance = (node.height - milestoneAncestorHeight);
            assert(milestoneAncestorDistance != 0);
            milestoneAncestorId = kthAncestor(parentNode, milestoneAncestorDistance - 1);
        }
        node.powerOf2HeightAncestors[m] = milestoneAncestorId;
        assert(milestoneAncestorId != node.id);
        assert(allNodes[milestoneAncestorId].height == milestoneAncestorHeight);
        assert(allNodes[node.powerOf2HeightAncestors[m]].height >= allNodes[parentNode.powerOf2HeightAncestors[m]].height);
    }
}


void updateHeightsAndAncestorTrackingAux(vector<Node>& allNodes, long nodeId, long parentNodeId, long height, vector<long>& ancestors)
{
    Node& node = allNodes[nodeId];
    node.height = height;
    if (parentNodeId != -1)
    {
        const Node& parentNode = allNodes[parentNodeId];
        updateAncestorTrackingFromParent(node, parentNode, ancestors);
    }
    ancestors.push_back(nodeId);
    for (auto childNodeId : node.children)
    {
        updateHeightsAndAncestorTrackingAux(allNodes, childNodeId, nodeId, height + 1, ancestors);
    }
    assert(ancestors.back() == nodeId);
    ancestors.pop_back();
}

void updateHeightsAndAncestorTracking(vector<Node>& allNodes, long rootNodeId)
{
    vector<long> ancestors;
    updateHeightsAndAncestorTrackingAux(allNodes, rootNodeId, -1, 0, ancestors);
}

long kthAncestorBruteForce(const Node& node, long k)
{
    if (!node.exists)
        return -1;
    long ancestorId = node.id;
    for (int i = 0; i < k; i++)
    {
        ancestorId = allNodes[ancestorId].parent;
        if (ancestorId == -1)
            return -1;
    }
    return ancestorId;
}

long kthAncestor(const Node& node, long k)
{
    if (!node.exists)
        return -1;
    assert(k >= 0);
    if (k == 0)
        return node.id;
    if (node.height < k)
        return -1;
    for (int m = maxHeightLog2 - 1; m >= 0; m--)
    {
        if (k >= (1 << m))
        {
            // Use our tracking info to jump up the ancestor list in logarithmic time :)
            const long milestoneAncestorId = node.powerOf2HeightAncestors[m];
            assert(milestoneAncestorId != node.id);
            const Node& milestoneAncestor = allNodes[milestoneAncestorId];
            return kthAncestor(milestoneAncestor, k - (node.height - milestoneAncestor.height));
        }
    }
    return -1;
}

enum NewNodeUpdateFlags
{
    UpdateNone = 0,
    UpdateParentChildren = 1,
    UpdateTracking = 2
};

void ensureCanHoldNodeId(const long nodeId)
{
    if (allNodes.size() < nodeId + 1)
    {
        allNodes.resize(nodeId + 1);
    }
}

void addNode(const long nodeId, const long parentId, int toUpdate)
{
    ensureCanHoldNodeId(nodeId);
    allNodes[nodeId] = Node(nodeId, parentId);
    if (parentId != -1)
    {
        Node& parentNode = allNodes[parentId];
        if ((toUpdate & UpdateParentChildren) != 0)
        {
            parentNode.children.push_back(nodeId);
        }
        if ((toUpdate & UpdateTracking) != 0)
        {
            updateAncestorTrackingFromParent(allNodes[nodeId], parentNode, vector<long>());
        }
    }
}

inline long readNodeId()
{
    long X;
    cin >> X;
    // Make 0-relative.
    X--;
    return X;
}


int main()
{
    int T;
    cin >> T;
    for (int t = 0; t < T; t++)
    {
        long P;
        cin >> P;

        // Basic approach: we add "ancestor tracking" information to each Node (via updateHeightsAndAncestorTracking):
        // this fills in the powerOf2HeightAncestors field in each Node, which stores, for each power of 2 we might
        // want to use, the id of the highest ancestor node whose height is a multiple of that power of 2.
        // This enables us to jump up the ancestor tree very quickly e.g. if the current node has height
        // 3075, then we can jump up to 3072 (3 * 1024) then to 2048 (2 * 1024) then to 1024 then to 0 (I think :))
        allNodes.clear();

        long rootNodeId = -1;
        for (long p = 0; p < P; p++)
        {
            const long X = readNodeId(), Y = readNodeId();
            const long maxNewNodeId = max(X, Y);
            ensureCanHoldNodeId(maxNewNodeId);
            addNode(X, Y, UpdateParentChildren);
            if (Y == -1)
            {
                assert(rootNodeId == -1);
                rootNodeId = X;
            }
        }
        Node& rootNode = allNodes[rootNodeId];
        fill(rootNode.powerOf2HeightAncestors.begin(), rootNode.powerOf2HeightAncestors.end(), rootNodeId);
        updateHeightsAndAncestorTracking(allNodes, rootNodeId);

        long Q;
        cin >> Q;
        for (long q = 0; q < Q; q++)
        {
            int instruction;
            cin >> instruction;
            switch (instruction)
            {
                case 0:
                    {
                        const long Y = readNodeId(), X = readNodeId();
                        addNode(X, Y, UpdateTracking);
                        break;
                    }
                case 1:
                    {
                        const long X = readNodeId();
                        Node& node = allNodes[X];
                        node.exists = false;
                        break;
                    }
                case 2:
                    {
                        const long X = readNodeId();
                        long K;
                        cin >> K;

                        Node& node = allNodes[X];
                        // Make un-0-relative :)
                        cout << (kthAncestor(node, K) + 1) << endl;
                        break;
                    }
                default:
                    assert(false);
            }
        }
    }
}

