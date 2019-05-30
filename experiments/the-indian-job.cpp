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



bool isSlotAssignmentPossible(SlotAssignment& slotAssignment, const vector<int>& timeNeeded, map<SlotAssignment, Possible>& possibilityLookup)
{
    if (slotAssignment.remainingTimeSlot1 < 0)
        return false;
    if (slotAssignment.remainingTimeSlot2 < 0)
        return false;
    if (slotAssignment.robberIndex == timeNeeded.size())
    {
        return true;
    }
    if (possibilityLookup[slotAssignment] != Possible::Unknown)
    {
        return (possibilityLookup[slotAssignment] == Possible::Yes);
    }

    bool possible = false;

    {
        SlotAssignment assignmentIfSlot1 = slotAssignment;
        assignmentIfSlot1.remainingTimeSlot1 -= timeNeeded[slotAssignment.robberIndex];
        assignmentIfSlot1.robberIndex++;

        possible = possible || isSlotAssignmentPossible(assignmentIfSlot1, timeNeeded, possibilityLookup);
    }
    {
        SlotAssignment assignmentIfSlot2 = slotAssignment;
        assignmentIfSlot2.remainingTimeSlot2 -= timeNeeded[slotAssignment.robberIndex];
        assignmentIfSlot2.robberIndex++;

        possible = possible || isSlotAssignmentPossible(assignmentIfSlot2, timeNeeded, possibilityLookup);
    }
    possibilityLookup[slotAssignment] = (possible ? Possible::Yes : Possible::No);

    return possible;
}

bool isSlotAssignmentPossible(int n, int g, const vector<int>& timeNeeded)
{
    map<SlotAssignment, Possible> possibilityLookup;
    SlotAssignment slotAssignment;
    slotAssignment.remainingTimeSlot1 = g;
    slotAssignment.remainingTimeSlot2 = g;
    slotAssignment.robberIndex = 0;
    return isSlotAssignmentPossible(slotAssignment, timeNeeded, possibilityLookup);

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

        const auto result = isSlotAssignmentPossible(n, g, timeNeeded);
        if (result)
            cout << "YES";
        else
            cout << "NO";
        cout << endl;
    }
}
