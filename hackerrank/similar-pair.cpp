#include <iostream>
#include <vector>
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

#define VERIFY_NUMBER_TRACKER

class NumberTracker
{
    public:
        NumberTracker(int maxNumber)
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
            //cout << "m_nextPowerOf2: " << m_nextPowerOf2 << endl;
#ifdef VERIFY_NUMBER_TRACKER
            m_numOfNumber.resize(maxNumber + 1);
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
            cout << " countNumbersInRange: start: " << start << " end: " << end << " num: " << num << " dbgNum: " << dbgNum << endl;
#endif
            return num;
        }
    private:
        int m_nextPowerOf2;
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
            //cout << "cell matrix:" << endl;
            for (int i = 0; i < m_cellMatrix.size(); i++)
            {
                //cout << "row: " << i << " ";
                for (const auto cell : m_cellMatrix[i])
                {
                    //cout << cell.numNumbersInRange << " ";
                }
                //cout << endl;
            }
#ifdef VERIFY_NUMBER_TRACKER
            m_numOfNumber[n] += (add ? 1 : 0);
#endif
        }
        int countNumbersInRange(int start, int end, int cellRow, int powerOf2)
        {
            //cout << " countNumbersInRange: start: " << start << " end: " << end << " cellRow: " << cellRow << " powerOf2: " << powerOf2 << endl;
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
            const int powerOf2AfterStart = ((start % powerOf2) == 0) ? start : (start / powerOf2 + 1) * powerOf2;
            const int powerOf2BeforeEnd = ((end % powerOf2) == 0) ? end : (end / powerOf2) * powerOf2;
            int numberInRange = 0;
            if ((start % powerOf2) == 0)
            {
                while (start <= end - powerOf2)
                {
                    const int cellContents = m_cellMatrix[cellRow][start/powerOf2].numNumbersInRange;
                    //cout << " from start, start = " << start << " end = " << end << " adding contents of cell " << cellRow << "," << start/powerOf2 << " = " << cellContents << endl;
                    numberInRange += cellContents;
                    start += powerOf2;
                }
                return numberInRange + countNumbersInRange(start, end, cellRow + 1, powerOf2 / 2);
            }
            if (((end + 1) % powerOf2) == 0)
            {
                while (start <= end - powerOf2)
                {
                    const int cellContents = m_cellMatrix[cellRow][end/powerOf2].numNumbersInRange;
                    //cout << " from end, start = " << start << " end = " << end << " adding contents of cell " << cellRow << "," << end/powerOf2 << " = " << cellContents << endl;
                    numberInRange += cellContents;
                    end -= powerOf2;
                }
                return numberInRange + countNumbersInRange(start, end, cellRow + 1, powerOf2 / 2);
            }
            for (int cellIndex = powerOf2AfterStart / powerOf2; cellIndex < powerOf2BeforeEnd / powerOf2; cellIndex++)
            {
                numberInRange += m_cellMatrix[cellRow][cellIndex].numNumbersInRange;
            }
            return numberInRange + countNumbersInRange(start, powerOf2AfterStart - 1, cellRow + 1, powerOf2 / 2) + countNumbersInRange(powerOf2BeforeEnd, end, cellRow + 1, powerOf2 / 2);
        }
        bool isPowerOf2(int n)
        {
            if ((n % 2) == 1)
                return false;
            return isPowerOf2(n / 2);
        }
#ifdef VERIFY_NUMBER_TRACKER
        vector<int> m_numOfNumber;
#endif
};


int main()
{
#if 1
    while (true)
    {
        const int trackerSize = 1000;
        NumberTracker tracker(trackerSize);
        const int randomNumber = rand() % trackerSize;
        cout << " randomNumber: " << randomNumber << endl;
        tracker.addNumber(randomNumber);
        for (int start = 0; start < trackerSize; start++)
        {
            for (int end = start; end < trackerSize; end++)
            {
                tracker.countNumbersInRange(start, end);
            }
        }
    }

    return 0;
#endif
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

#define BRUTE_FORCE
#ifdef BRUTE_FORCE
    vector<Node*> ancestors;
    const int numSimilarPairsBruteForce = ::numSimilarPairsBruteForce(root, k, ancestors);
    cout << numSimilarPairsBruteForce << endl;
#endif

}

