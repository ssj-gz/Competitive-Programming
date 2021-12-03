#include <iostream>
#include <vector>
#include <sstream>
#include <limits>
#include <cassert>

using namespace std;

int main()
{
    int64_t earliestDepartureDate;
    cin >> earliestDepartureDate;
    string busIdsLine;
    cin >> busIdsLine;
    istringstream busIdsStream(busIdsLine);
    vector<int64_t> busIds;
    while (busIdsStream)
    {
        char peekChar;
        busIdsStream >> peekChar;

        if (peekChar != 'x')
        {
            busIdsStream.putback(peekChar);
            int64_t busId = -1;
            busIdsStream >> busId;
            assert(busId != -1);
            busIds.push_back(busId);
        }
        else
        {
            busIds.push_back(-1);
        }

        char dummyChar = '\0';
        busIdsStream >> dummyChar;
        if (busIdsStream.eof())
            break;
        assert(dummyChar == ',');
    }
    assert(busIdsStream.eof());

    cout << "bus ids: " << endl;
    for (const auto x : busIds)
    {
        cout << x << endl;
    }

    int64_t time = 0;
    int64_t increment = 1;
    int64_t numAttempts = 0;
    int nextBusIndexToFindTimeFor = 0;
    while (true)
    {
        int busIndex = -1;
        bool isRequiredTime = true;
        cout << "time: " << time << " nextBusIndexToFindTimeFor: " << nextBusIndexToFindTimeFor << " numAttempts: " << numAttempts << endl;
        for (const auto busId : busIds)
        {
            busIndex++;

            if (busId == -1)
            {
                if (nextBusIndexToFindTimeFor == busIndex)
                    nextBusIndexToFindTimeFor++;
                continue;
            }

            const int64_t requiredDepartureTime = time + busIndex;
            if (requiredDepartureTime % busId != 0)
            //if (nextArrivalTime - time != busIndex)
            {
                isRequiredTime = false;
                //cout << "  nope" << endl;
                if (nextBusIndexToFindTimeFor == busIndex)
                {
                    //cout << " was " << (nextArrivalTime - time) << " should be " << busIndex << endl;
                    cout << " drat: " << busIndex << " " << busId << " was: " << (requiredDepartureTime % busId ) << endl;
                }
                break;
            }
            else
            {
                if (busIndex == nextBusIndexToFindTimeFor)
                {
                    increment = increment * busId;
                    cout << "increment now: " << increment << endl;
                    nextBusIndexToFindTimeFor++;
                }

            }
            //cout << " Next arrival time for busId " <<  busId << " index: " << busIndex << endl;
            //cout << "Blah for bus Id: " << busId << " (index: " << busIndex << ") = " << (nextArrivalTime - time) << " should be " << busIndex << endl;
        }
        if (isRequiredTime)
        {
            cout << "found time: " << time << endl;
            break;
        }
        time += increment;
        numAttempts++;
        if (time % 10'000'000 == 0)
            cout << "time: " << time << endl;
    }
#if 0
    int64_t earliestBusTime = numeric_limits<int64_t>::max();
    int64_t earliestBusId = -1;
    for (const auto& busId : busIds)
    {
        int64_t nextArrivalTime = (earliestDepartureDate % busId == 0) ? earliestDepartureDate : ((earliestDepartureDate / busId) + 1) * busId;
        if (nextArrivalTime < earliestBusTime)
        {
            earliestBusTime = nextArrivalTime;
            earliestBusId = busId;
        }
    }
    assert(earliestBusId != -1);
    cout << (earliestBusTime - earliestDepartureDate) * earliestBusId  << endl;
#endif
}
