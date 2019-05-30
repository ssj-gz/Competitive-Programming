// Simon St James (ssjgz) - 2019-05-30
#include <iostream>
#include <vector>
#include <map>

using namespace std;

enum class Possible { Unknown, Yes, No };

struct SlotAssignment
{
    int remainingTimeSlot1 = -1;
    int remainingTimeSlot2 = -1;
    int robberIndex = -1;
    bool operator<(const SlotAssignment& other) const
    {
        if (remainingTimeSlot1 != other.remainingTimeSlot1)
            return remainingTimeSlot1 < other.remainingTimeSlot1;
        if (remainingTimeSlot2 != other.remainingTimeSlot2)
            return remainingTimeSlot2 < other.remainingTimeSlot2;
        return robberIndex < other.robberIndex;
    }
};



bool solveNaive(int remainingTimeSlot1, int remainingTimeSlot2, int robberIndex, const vector<int>& timeNeeded, map<SlotAssignment, Possible>& possibilityLookup)
{
    //cout << "remainingTimeSlot1: " << remainingTimeSlot1 << " remainingTimeSlot2: " << remainingTimeSlot2 << " robberIndex: " << robberIndex <<  endl;
    if (remainingTimeSlot1 < 0)
        return false;
    if (remainingTimeSlot2 < 0)
        return false;
    if (robberIndex == timeNeeded.size())
    {
        return true;
    }
    SlotAssignment a;
    a.remainingTimeSlot1 = remainingTimeSlot1;
    a.remainingTimeSlot2 = remainingTimeSlot2;
    a.robberIndex = robberIndex;
    if (possibilityLookup[a] != Possible::Unknown)
    {
        return (possibilityLookup[a] == Possible::Yes);
    }

    bool possible = false;

    {
        SlotAssignment aIfSlot1;
        aIfSlot1.remainingTimeSlot1 = remainingTimeSlot1 - timeNeeded[robberIndex];
        aIfSlot1.remainingTimeSlot2 = remainingTimeSlot2;
        aIfSlot1.robberIndex = robberIndex + 1;

        possible = possible || solveNaive(aIfSlot1.remainingTimeSlot1, aIfSlot1.remainingTimeSlot2, aIfSlot1.robberIndex, timeNeeded, possibilityLookup);
    }
    {
        SlotAssignment aIfSlot2;
        aIfSlot2.remainingTimeSlot1 = remainingTimeSlot1;
        aIfSlot2.remainingTimeSlot2 = remainingTimeSlot2 - timeNeeded[robberIndex];
        aIfSlot2.robberIndex = robberIndex + 1;

        possible = possible || solveNaive(aIfSlot2.remainingTimeSlot1, aIfSlot2.remainingTimeSlot2, aIfSlot2.robberIndex, timeNeeded, possibilityLookup);
    }
    possibilityLookup[a] = (possible ? Possible::Yes : Possible::No);

    return possible;
}

bool solveNaive(int n, int g, const vector<int>& timeNeeded)
{
    map<SlotAssignment, Possible> possibilityLookup;
    return solveNaive(g, g, 0, timeNeeded, possibilityLookup);

}

int main(int argc, char* argv[])
{
    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        int n, g;
        cin >> n >> g;

        vector<int> timeNeeded(n);
        for (int i = 0; i < n; i++)
        {
            cin >> timeNeeded[i];
        }

        const auto result = solveNaive(n, g, timeNeeded);
        if (result)
            cout << "YES";
        else
            cout << "NO";
        cout << endl;
    }
}
