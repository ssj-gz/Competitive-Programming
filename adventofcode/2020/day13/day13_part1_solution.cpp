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
}
