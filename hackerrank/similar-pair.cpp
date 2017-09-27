// Simon St James (ssjgz) 2017-09-27 11:31
#define BRUTE_FORCE
#define VERIFY_NUMBER_TRACKER
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#undef BRUTE_FORCE
#undef VERIFY_NUMBER_TRACKER
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


int numSimilarPairsBruteForce(Node* root, int k, vector<Node*>& ancestors)
{
    int numSimilarPairs = 0;
    for (const auto& ancestor : ancestors)
    {
        if (abs(root->nodeNumber - ancestor->nodeNumber) <= k)
        {
            numSimilarPairs++;
        }
    }
    ancestors.push_back(root);
    for (auto child : root->children)
    {
        numSimilarPairs += numSimilarPairsBruteForce(child, k, ancestors);

    }
    ancestors.pop_back();
    return numSimilarPairs;

}

class NumberTracker
{
    public:
        NumberTracker(int maxNumber)
            : m_maxNumber{maxNumber}
        {
            int log2 = 0;
            int powerOf2 = 1;
            while (maxNumber > powerOf2)
            {
                log2++;
                powerOf2 *= 2;
            }
            m_nextPowerOf2 = powerOf2;
            //if (powerOf2 != maxNumber)
            //{
                //m_nextPowerOf2 *= 2;
            //}
            int numCellsInThisRow = 1;
            powerOf2 = m_nextPowerOf2;
            while (powerOf2 > 0)
            {
                m_cellMatrix.push_back(vector<Cell>(numCellsInThisRow));
                numCellsInThisRow *= 2;
                powerOf2 /= 2;
            }
#ifdef VERIFY_NUMBER_TRACKER
            m_numOfNumber.resize(m_nextPowerOf2 + 1);
#endif
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
            const int num = countNumbersInRange(start, end, 0, m_nextPowerOf2);
#ifdef VERIFY_NUMBER_TRACKER
            int dbgNum = 0;
            for (int i = start; i <= end; i++)
            {
                dbgNum += m_numOfNumber[i];
            }
            assert(num == dbgNum);
#endif
            return num;
        }
        int maxNumber() const
        {
            return m_maxNumber;
        }
    private:
        int m_nextPowerOf2;
        int m_maxNumber;
        struct Cell
        {
            int numNumbersInRange = 0;
        };
        vector<vector<Cell>> m_cellMatrix;
        void addOrRemoveNumber(int n, bool add)
        {
            int cellRow = 0;
            int powerOf2 = m_nextPowerOf2;
            while (cellRow < m_cellMatrix.size())
            {
                int cellIndex = n / powerOf2;
                m_cellMatrix[cellRow][cellIndex].numNumbersInRange += (add ? 1 : -1);

                cellRow++;
                powerOf2 /= 2;
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
                return countNumbersInRange(start, end, cellRow + 1, powerOf2 / 2);
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
                return numberInRange + countNumbersInRange(start, end, cellRow + 1, powerOf2 / 2);
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
                return numberInRange + countNumbersInRange(start, end, cellRow + 1, powerOf2 / 2);
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
            return numberInRange + countNumbersInRange(start, powerOf2AfterStart - 1, cellRow + 1, powerOf2 / 2) + countNumbersInRange(powerOf2BeforeEnd, end, cellRow + 1, powerOf2 / 2);
        }
        bool isPowerOf2(int n)
        {
            if ((n % 2) == 1)
                return false;
            return isPowerOf2(n / 2);
        }
};

int64_t numSimilarPairs(Node* root, int k, NumberTracker& numberTracker)
{
    struct StackFrame
    {
        int childIndex = -1;
        int64_t numSimilarPairs = 0;
        bool waitingForChild = false;
        Node* currentNode = nullptr;
    };
    stack<StackFrame> stackFrames;

    StackFrame initialStackFrame;
    initialStackFrame.currentNode = root;
    stackFrames.push(initialStackFrame);


    int64_t childReturnValue = -1;
    while (!stackFrames.empty())
    {
        StackFrame& currentStackFrame = stackFrames.top();
        //cout << "stack frames size: " << stackFrames.size() << " current node: " << currentStackFrame.currentNode << " child: " << currentStackFrame.childIndex << " # children: " << currentStackFrame.currentNode->children.size() << endl;
        if (currentStackFrame.childIndex == -1)
        {
            currentStackFrame.numSimilarPairs = numberTracker.countNumbersInRange(max(0, currentStackFrame.currentNode->nodeNumber - k), min(currentStackFrame.currentNode->nodeNumber + k, numberTracker.maxNumber()));
            //cout << " currentStackFrame.numSimilarPairs: " << currentStackFrame.numSimilarPairs << endl;
            currentStackFrame.childIndex = 0;
            numberTracker.addNumber(currentStackFrame.currentNode->nodeNumber);
        }
        if (currentStackFrame.waitingForChild)
        {
            currentStackFrame.waitingForChild = false;
            assert(childReturnValue != -1);
            currentStackFrame.numSimilarPairs += childReturnValue;
            currentStackFrame.childIndex++;
            childReturnValue = -1;
        }
        if (currentStackFrame.childIndex != currentStackFrame.currentNode->children.size())
        {
            Node* child = currentStackFrame.currentNode->children[currentStackFrame.childIndex];
            //cout << " node: " << currentStackFrame.currentNode << " pushing child: " << child << endl;
            StackFrame nextStackFrame;
            nextStackFrame.currentNode = child;
            currentStackFrame.waitingForChild = true;
            stackFrames.push(nextStackFrame);
        }
        else
        {
            childReturnValue = currentStackFrame.numSimilarPairs;
            numberTracker.removeNumber(currentStackFrame.currentNode->nodeNumber);
            stackFrames.pop();
        }
    }


    return childReturnValue;

}



int main()
{
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
    NumberTracker numberTracker(n);
    const int64_t numSimilarPairs = ::numSimilarPairs(root, k, numberTracker);
    cout << numSimilarPairs << endl;

}

