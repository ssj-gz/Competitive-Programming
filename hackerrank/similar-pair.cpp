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
                // Gap between start and end is too small; recurse with next row.
                return countNumbersInRange(start, end, cellRow + 1, powerOf2 >> 1);
            }
            int numberInRange = 0;
            if ((start % powerOf2) == 0)
            {
                // Advance start by one complete cell at a time, summing the contents of that cell, then recurse on the rest.
                if (start <= end - powerOf2)
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
                if (start <= end - powerOf2)
                {
                    const int cellContents = m_cellMatrix[cellRow][end/powerOf2].numNumbersInRange;
                    numberInRange += cellContents;
                    end -= powerOf2;
                }
                return numberInRange + countNumbersInRange(start, end, cellRow + 1, powerOf2 >> 1);
            }
            // Neither start nor end is a multiple of powerOf2 ... sum up the complete cells in between (if any) ...
            const int powerOf2AfterStart = (start / powerOf2 + 1) * powerOf2;
            const int powerOf2BeforeEnd =  (end / powerOf2) * powerOf2;
            // ... add the complete cell in between (if there is one) ...
            if (powerOf2AfterStart < powerOf2BeforeEnd)
            {
                numberInRange += m_cellMatrix[cellRow][powerOf2AfterStart / powerOf2].numNumbersInRange;
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
    // Fairly easy one - I actually solved it in my head ages ago (sometime in 2016, I think) before I knew about
    // segment trees and whatnot.  The problem description tells you to look into Binary Indexed Trees but, of course,
    // I consider that cheating and wanted to figure out my own approach :)
    // So: if we could do a DFS and, in sub-linear time, find out how many ancestor nodes had nodeNumbers in the range
    // currentNode->nodeNumber - k ... currentNode->nodeNumber + k (inclusive), we'd be done.  A kind of "NumberTracker" data structure where
    // we can add a number (when the node with that nodeNumber is added to the ancestor stack); check how many numbers 
    // have been added in a range; and then remove the number (when the node with that nodeNumber is popped from the stack)
    // each in O(log(N)) would be nice :)
    //
    // The approach I used is as follows.  Find the maximum number we wish to store (N), and find the lowest power of 2
    // greater than or equal to that - e.g. if the maxNumber is 2000, we pick 2048.  Create a matrix of "cells" where
    // each cell represents a range.  Well, it's not really a matrix since each row has a different number of columns,
    // but that's close enough :)  The topmost row consists of a single cell which (in our example where 2000 is the max) 
    // represents the range 0 ... 2047. The next row has two cells (twice as many as the last): one representing 0 ... 1023, 
    // the other 1024 ... 2047. The next row has four cells (again, twice as many as the last), representing 0 ... 511,
    // 512 ... 1023, 1024 ... 1535 and 1536 ... 2047, respectively.  In general, each row has twice as many cells as the previous
    // one, and each cell represents a range half the size of those on the previous row, until we get rows of width 1 in the final
    // row.  The number of rows is O(log2(2048)) ~ O(log(N)).
    //
    // Let's add a node with nodeNumber 300 to our ancestorTracker.  This would add 1 to the numNumbersInRange to the cell in the first row
    // representing 0 ... 2047; 1 to the cell in the second row representing 0 ... 511 (the first in the row); 1 to the cell in the third row
    // representing 256 ... 511 (the second in the row); 1 to the cell in the fourth row representing 256 ... 383 (the third in the row)
    // etc, all the way down to adding 1 to the 300th cell in the last row.  Likewise, removing 300 from the ancestorTracker would subtract
    // 1 from the same set of cells.  Adding or removing a number from our NumberTracker takes O(log(N)) (we process one cell per row, and there
    // are O(log(N)) rows).
    //
    // How do we find how many numbers were added in a given range? Say we've added 300, and want to count the numbers in the range 212 - 312.
    // The cell in the first row tells us that there is one number in the range 0 ... 2047, which doesn't really help! Let's drop to the next row: 
    // The first cell in the second row tells us that there is one number in the range 0 ... 1023 (we're not interested in the range 
    // 1024 ... 2047, obviously!), which again doesn't really help us - let's try the next row.   This tells us that is a number in the range 0 ... 511; still
    // not very helpful; let's try the next row.  This tells us that there is 1 in the range 256 ... 511 (and none elsewhere); still not that helpful; let's try the next
    // row.  This tells us that there is 1 in the range 256 ... 384; getting closer - next row!
    //
    // The next row tells us that there is 1 in the range 256 ... 319; still not that helpful; next row! The next row tells us that there is 1 in the range 288 - 319.
    // The next row tells us that there is 1 in the range 288 ... 303, and none elsewhere.  Next row - 1 in the range 296 - 304, none elsewhere.
    // Next row - etc, etc.  Actually, this was a pretty bad example, but you can see that you can gradually narrow down a range as we go down the rows.
    // In a properly chosen example, we'd see that we can often narrow down the range dramatically i.e. if the range was 256 - 1023, then by the time we get to the 
    // third row, the number in the range is given by the number in the range (256 ... 511) plus the number in the range (512 ... 1023), but the latter is 
    // just numNumbersInRange for the 2nd cell in the row, so now we just have to compute the number in the range 256 ... 511.
    // Anyway, that's about it - sorry for the crap example!

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

