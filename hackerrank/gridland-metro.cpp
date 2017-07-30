#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include <map>
#include <set>
#include <list>
using namespace std;

class Train;

bool operator<(const Train& lhs, const Train& rhs);
class Train
{
    public:
        Train(int64_t columnBegin, int64_t columnEnd)
            : m_columnBegin(columnBegin),
            m_columnEnd(columnEnd)
    {

    }
        Train mergedWith(const Train& other)
        {
            Train train1(*this);
            Train train2(other);
            if (train2 < train1)
                swap(train1, train2);
            if (train2.columnBegin() > train1.columnEnd() + 1)
                return Train(-1, -1);
            return Train(min(train1.columnBegin(), train2.columnBegin()), max(train1.columnEnd(), train2.columnEnd()));
        }
        bool isValid() const
        {
            return m_columnBegin != -1 && m_columnEnd != -1;
        }
        int64_t length() const
        {
            return m_columnEnd - m_columnBegin + 1;
        }
        int64_t columnBegin() const
        {
            return m_columnBegin;    
        }
        int64_t columnEnd() const
        {
            return m_columnEnd;    
        }
    private:
        int64_t m_columnBegin;
        int64_t m_columnEnd;
};
bool operator<(const Train& lhs, const Train& rhs)
{
    return lhs.columnBegin() < rhs.columnBegin();
}
ostream& operator<<(ostream& os, const Train& train)
{
    os << "Train (" << train.columnBegin() << ", " << train.columnEnd() << ")" << endl;
    return os;
}

int main() {
    int64_t n, m, k;
    cin >> n >> m >> k;
    // The merged trains on a row are stored in order of columnBegin().  No
    // merged trains overlap, nor any any immediately adjacent to each other.
    map<int64_t, set<Train>> mergedTrainsOnRow;
    for (int64_t i = 0; i < k; i++)
    {
        int64_t r, c1, c2;
        cin >> r >> c1 >> c2;
        set<Train>& currentTrainsOnRow = mergedTrainsOnRow[r];
        Train mergedNewTrain(c1, c2);
        // Merge with existing Train in this row? The candidates for merging 
        // are the first train whose columnBegin() is less than the new Train's 
        // columnBegin() (if there is one), then all subsequent trains, in order of increasing
        // columnBegin(), whose columnBegin() is not too far past new Train's columnEnd().
        auto trainIter = currentTrainsOnRow.lower_bound(mergedNewTrain);
        if (trainIter == currentTrainsOnRow.end() && !currentTrainsOnRow.empty())
            trainIter--; // All trains have columnBegin() < new Train's columnBegin(); find Train with highest.
        if (trainIter != currentTrainsOnRow.end())
        {
            if (trainIter != currentTrainsOnRow.begin())
            {
                // We've found a train with columnBegin() >= new Train's columnBegin(); find train with
                // highest columnBegin() which is < new Train's columnBegin().  This will be the previous train.
                trainIter--; 
            }
            list<Train> absorbedTrainsToRemove;
            while (trainIter != currentTrainsOnRow.end() && trainIter->columnBegin() <= mergedNewTrain.columnEnd() +1)
            {
                const Train& existingTrain = *trainIter;
                if (mergedNewTrain.mergedWith(existingTrain).isValid())    
                {
                    mergedNewTrain = mergedNewTrain.mergedWith(existingTrain);
                    absorbedTrainsToRemove.push_back(existingTrain);
                }
                trainIter++;
            }
            for (const auto& trainToRemove : absorbedTrainsToRemove)
                currentTrainsOnRow.erase(trainToRemove);
        }

        currentTrainsOnRow.insert(mergedNewTrain);
    }
    int64_t possibleLampPostSites = n * m;
    for (const auto& rowAndtrainsOnRow : mergedTrainsOnRow )
    {
        for (const auto& train : rowAndtrainsOnRow.second)
        {
            possibleLampPostSites -= train.length();
        }
    }
    cout << possibleLampPostSites;
    return 0;
}

