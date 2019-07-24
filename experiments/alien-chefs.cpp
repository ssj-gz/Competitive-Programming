// Simon St James (ssjgz) - 2019-XX-XX
#include <iostream>
#include <vector>
#include <set>

#include <cassert>

#include <sys/time.h>

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

struct Range
{
    int start = -1;
    int end = -1;
    int rangeId = -1;
};

bool operator<(const Range& lhs, const Range& rhs)
{
    if (lhs.start != rhs.start)
        return lhs.start < rhs.start;
    if (lhs.end != rhs.end)
        return lhs.end < rhs.end;

    return lhs.rangeId < rhs.rangeId;
}

struct Query
{
    int numAliens = -1;
    vector<int> alienTimes;
};

vector<int> solveBruteForce(const vector<Range>& ranges, const vector<Query>& queries)
{
    vector<int> resultForQuery;

    for (const auto& query : queries)
    {
        set<Range> rangesForQuery;
        for (auto alienTime : query.alienTimes)
        {
            for (const auto& range : ranges)
            {
                if (alienTime >= range.start && alienTime <= range.end)
                {
                    rangesForQuery.insert(range);
                }
            }
        }
        resultForQuery.push_back(rangesForQuery.size());
    }

    return resultForQuery;
}

int main(int argc, char* argv[])
{
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int numRanges = rand() % 100 + 1;
        const int maxTime = rand() % 1000 + 2;
        const int numQueries = rand() % 100 + 1;
        const int maxInGroup = rand() % 20 + 1;

        cout << numRanges << endl;
        for (int i = 0; i < numRanges; i++)
        {
            int rangeStart = -1;
            int rangeEnd = -1;
            while (true)
            {
                rangeStart = (rand() % maxTime) + 1;
                rangeEnd = (rand() % maxTime) + 1;
                if (rangeStart != rangeEnd)
                {
                    if (rangeEnd < rangeStart)
                        swap(rangeStart, rangeEnd);
                    break;
                }
            }
            cout << rangeStart << " " << rangeEnd << endl;
        }
        
        cout << numQueries << endl;
        for (int i = 0; i < numQueries; i++)
        {
            const int numInGroup = (rand() & maxInGroup) + 1;
            cout << numInGroup << " ";
            for (int j = 0; j < numInGroup; j++)
            {
                cout << ((rand() % maxTime) + 1) << " ";
            }
            cout << endl;
        }


        return 0;
    }

    const int numRanges = read<int>();

    vector<Range> ranges(numRanges);
    for (int i = 0; i < numRanges; i++)
    {
        ranges[i].start = read<int>();
        ranges[i].end = read<int>();
        assert(ranges[i].start < ranges[i].end);
        ranges[i].rangeId = i;
    }

    int numQueries;
    cin >> numQueries;
    vector<Query> queries(numQueries);

    for (auto& query : queries)
    {
        query.numAliens = read<int>();
        for (int i = 0; i < query.numAliens; i++)
        {
            query.alienTimes.push_back(read<int>());
        }
    }
    
    const auto solutionBruteForce = solveBruteForce(ranges, queries);
    cout << "solutionBruteForce: ";
    for (const auto& x : solutionBruteForce)
    {
        cout << x << " ";
    }
    cout << endl;


}

