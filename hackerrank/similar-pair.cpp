// Simon St James (ssjgz) 2017-09-27 11:31
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <stack>
#include <cassert>

using namespace std;

struct Node
{
    vector<Node*> children;
    int nodeNumber = -1;
    bool isRoot = true;
};

// Add or remove a number 0< = n <= maxNumber in O(log(maxNumber)); count number of numbers
// in range start,end (inclusive) in O(log(maxNumber))
class NumberTracker
{
    public:
        NumberTracker(int maxNumber)
            : m_maxNumber{maxNumber}
        {
            int exponentOfPowerOf2 = 0;
            int powerOf2 = 1;
            while (maxNumber > powerOf2)
            {
                exponentOfPowerOf2++;
                powerOf2 *= 2;
            }
            m_powerOf2BiggerThanMaxNumber = powerOf2;
            m_exponentOfPowerOf2BiggerThanMaxNumber = exponentOfPowerOf2;
            // Build cell matrix.
            int numCellsInThisRow = 1;
            powerOf2 = m_powerOf2BiggerThanMaxNumber;
            while (powerOf2 > 0)
            {
                m_cellMatrix.push_back(vector<Cell>(numCellsInThisRow));
                numCellsInThisRow *= 2;
                powerOf2 /= 2;
            }
        }
        void addNumber(int n)
        {
            addOrRemoveNumber(n, true);
        }
        void removeNumber(int n)
        {
            addOrRemoveNumber(n, false);
        }
        int countNumbersInRange(int start, int end)
        {
            start = max(0, start); // Snap start and end to interval.
            end  = min(end, m_maxNumber);
            return countNumbersInRange(start, end, 0, m_powerOf2BiggerThanMaxNumber);
        }
    private:
        int m_powerOf2BiggerThanMaxNumber;
        int m_exponentOfPowerOf2BiggerThanMaxNumber;
        int m_maxNumber;
        struct Cell
        {
            int numNumbersInRange = 0;
        };
        vector<vector<Cell>> m_cellMatrix;
        void addOrRemoveNumber(int n, bool add)
        {
            int cellRow = 0;
            int exponentOfPowerOf2 = m_exponentOfPowerOf2BiggerThanMaxNumber;
            const int numToAdd = (add ? 1 : -1);
            for (auto& cellRow : m_cellMatrix)
            {
                int cellIndex = n >> exponentOfPowerOf2;
                cellRow[cellIndex].numNumbersInRange += numToAdd;

                exponentOfPowerOf2--;
            }
        }
        int countNumbersInRange(int start, int end, int cellRow, int powerOf2)
        {
            if (end < start)
                return 0;
            if (end == start)
            {
                return m_cellMatrix.back()[start].numNumbersInRange;
            }
            if (cellRow == m_cellMatrix.size())
                return 0;
            if (end + 1 - start < powerOf2)
            {
                // Gap between start and end is too small; recurse with smaller power of 2.
                return countNumbersInRange(start, end, cellRow + 1, powerOf2 >> 1);
            }
            int numberInRange = 0;
            if ((start % powerOf2) == 0)
            {
                // Advance start by one complete cell at a time, summing the contents of that cell, then recurse on the rest.
                while (start <= end - powerOf2)
                {
                    const int cellContents = m_cellMatrix[cellRow][start/powerOf2].numNumbersInRange;
                    numberInRange += cellContents;
                    start += powerOf2;
                }
                return numberInRange + countNumbersInRange(start, end, cellRow + 1, powerOf2 >> 1);
            }
            if (((end + 1) % powerOf2) == 0)
            {
                // Unadvance end by one complete cell at a time, summing the contents of that cell, then recurse on the rest.
                while (start <= end - powerOf2)
                {
                    const int cellContents = m_cellMatrix[cellRow][end/powerOf2].numNumbersInRange;
                    numberInRange += cellContents;
                    end -= powerOf2;
                }
                return numberInRange + countNumbersInRange(start, end, cellRow + 1, powerOf2 >> 1);
            }
            // Neither start nor end is a multiple of powerOf2: sum up the complete cells in between (if any) ...
            const int powerOf2AfterStart = (start / powerOf2 + 1) * powerOf2;
            const int powerOf2BeforeEnd =  (end / powerOf2) * powerOf2;
            for (int cellIndex = powerOf2AfterStart / powerOf2; cellIndex < powerOf2BeforeEnd / powerOf2; cellIndex++)
            {
                numberInRange += m_cellMatrix[cellRow][cellIndex].numNumbersInRange;
            }
            // ... and then split into two, and recurse: for each of two split regions, at least one of the start or end will be a multiple of powerOf2, so they
            // will not split further.
            return numberInRange + countNumbersInRange(start, powerOf2AfterStart - 1, cellRow + 1, powerOf2 >> 1) + countNumbersInRange(powerOf2BeforeEnd, end, cellRow + 1, powerOf2 >> 1);
        }
};

int64_t numSimilarPairs(Node* root, int k, NumberTracker& ancestorTracker)
{
    // This would be a nice, simple recursive function looking like this:
    //
    //    int64_t numSimilarPairs(Node* root, int k, NumberTracker& ancestorTracker)
    //    {
    //        int64_t numSimilarPairs = ancestorTracker.countNumbersInRange(root->nodeNumber - k, root->nodeNumber + k);
    //        ancestorTracker.addNumber(root->nodeNumber);
    //        for (auto child : root->children)
    //        {
    //            numSimilarPairs += ::numSimilarPairs(child, k, numberTracker);
    //    
    //        }
    //        ancestorTracker.removeNumber(root->nodeNumber);
    //        return numSimilarPairs;
    //    }
    //
    // but unfortunately, the depth of the graph is too big and causes a stack overflow :/ So we have to emulate recursion ourselves.
    struct StackFrame
    {
        int childIndex = -1;
        Node* currentNode = nullptr;
    };
    stack<StackFrame> stackFrames;

    StackFrame initialStackFrame;
    initialStackFrame.currentNode = root;
    stackFrames.push(initialStackFrame);

    int64_t numSimilarPairs = 0;

    int64_t childReturnValue = -1;
    while (!stackFrames.empty())
    {
        StackFrame& currentStackFrame = stackFrames.top();
        if (currentStackFrame.childIndex == -1)
        {
            // First time dealing with this node/ stack frame.
            numSimilarPairs += ancestorTracker.countNumbersInRange(currentStackFrame.currentNode->nodeNumber - k, currentStackFrame.currentNode->nodeNumber + k);
            currentStackFrame.childIndex = 0;
            ancestorTracker.addNumber(currentStackFrame.currentNode->nodeNumber);
        }
        if (currentStackFrame.childIndex != currentStackFrame.currentNode->children.size())
        {
            // Process this child.
            Node* child = currentStackFrame.currentNode->children[currentStackFrame.childIndex];
            StackFrame nextStackFrame;
            nextStackFrame.currentNode = child;
            stackFrames.push(nextStackFrame);
            currentStackFrame.childIndex++;
        }
        else
        {
            // Finished with this stack frame; pop the stackframe from the stack and the node from the
            // ancestor tracker.
            ancestorTracker.removeNumber(currentStackFrame.currentNode->nodeNumber);
            stackFrames.pop();
        }
    }


    return numSimilarPairs;

}

int main()
{
    ios::sync_with_stdio(false);
    int n, k;
    cin >> n >> k;

    vector<Node> nodes(n);
    for (int i = 0; i < n; i++)
    {
        nodes[i].nodeNumber = i + 1;
    }

    for (int i = 0; i < n - 1; i++)
    {
        int parent, child;
        cin >> parent >> child;

        parent--;
        child--;

        nodes[parent].children.push_back(&(nodes[child]));
        nodes[child].isRoot = false;
    }

    Node* root = nullptr;
    for (auto& node : nodes)
    {
        if (node.isRoot)
        {
            assert(!root);
            root = &node;
        }
    }
    assert(root);
    NumberTracker ancestorTracker(n);
    const int64_t numSimilarPairs = ::numSimilarPairs(root, k, ancestorTracker);
    cout << numSimilarPairs << endl;

}

