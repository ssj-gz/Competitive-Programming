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
    int firstRemainingRobberIndex = -1;
    bool operator<(const SlotAssignment& other) const
    {
        if (remainingTimeSlot1 != other.remainingTimeSlot1)
            return remainingTimeSlot1 < other.remainingTimeSlot1;
        if (remainingTimeSlot2 != other.remainingTimeSlot2)
            return remainingTimeSlot2 < other.remainingTimeSlot2;
        return firstRemainingRobberIndex < other.firstRemainingRobberIndex;
    }
};

// Can all robbers beginning with slotAssignment.firstRemainingRobberIndex fit into
// slotAssignment.remainingTimeSlot1 and slotAssignment.remainingTimeSlot2?
bool isSlotAssignmentPossible(SlotAssignment& slotAssignment, const vector<int>& timeNeeded, map<SlotAssignment, Possible>& possibilityLookup)
{
    if (slotAssignment.remainingTimeSlot1 < 0)
        return false;
    if (slotAssignment.remainingTimeSlot2 < 0)
        return false;
    if (slotAssignment.firstRemainingRobberIndex == timeNeeded.size())
    {
        // No robbers left; some slot time left - we made it!
        return true;
    }
    if (possibilityLookup[slotAssignment] != Possible::Unknown)
    {
        return (possibilityLookup[slotAssignment] == Possible::Yes);
    }

    bool possible = false;
    {
        // Robber takes slot 1.
        SlotAssignment assignmentIfSlot1 = slotAssignment;
        assignmentIfSlot1.remainingTimeSlot1 -= timeNeeded[slotAssignment.firstRemainingRobberIndex];
        assignmentIfSlot1.firstRemainingRobberIndex++;

        possible = possible || isSlotAssignmentPossible(assignmentIfSlot1, timeNeeded, possibilityLookup);
    }
    {
        // Robber takes slot 2.
        SlotAssignment assignmentIfSlot2 = slotAssignment;
        assignmentIfSlot2.remainingTimeSlot2 -= timeNeeded[slotAssignment.firstRemainingRobberIndex];
        assignmentIfSlot2.firstRemainingRobberIndex++;

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
    slotAssignment.firstRemainingRobberIndex = 0;
    return isSlotAssignmentPossible(slotAssignment, timeNeeded, possibilityLookup);

}

int main(int argc, char* argv[])
{
    // Hmmm ... not happy with this one, as I prepared a "naive" way of doing this
    // which I expected to cause an out-of-memory error in the worse case, but
    // it looks like it actually works just fine - either because my analysis
    // was naive and that this actually *can't* cause an OOM, or because the
    // testcases are weak XD.  For the record, I wasn't able to contrive a testcase
    // that got even close to an OOM, so who knows?
    //
    // I might come back and re-visit it at some point with a better (provable) worst-case 
    // efficiency, but for now - this is was a very easy 70 points XD
    //
    // So, a few observations:
    //
    //   1) The situation can be modelled as having two "slots" in the bank vault, with
    //      each slot holding at most one robber at a time.
    //     
    //   2) There is no benefit to having either of the slots empty at any point - thus,
    //      when one robber has finished, he should be immediately replaced with another
    //      robber if any are left.
    //  
    //   3) The order in which the robbers take turns is irrelevant, so we can assume they
    //      go in the given order.
    //
    // If we could answer the following question:
    //   
    //   can all robbers starting at firstRemainingRobberIndex fit into the slots, if slot 1
    //   has remainingTimeSlot1 time remaining and slot 2 has remainingTimeSlot2 time remaining
    //   get all the time they need?
    //
    // We'd have a solution.
    //
    // Call this triple of firstRemainingRobberIndex, remainingTimeSlot1 and remainingTimeSlot2 a
    // "Slot Assignment".  Then it's hopefully obvious that we can construct a simple recurrence
    // relation by first allowing robber firstRemainingRobberIndex to occupy slot 1 (and seeing if
    // the resulting slot assignment is possible) and then allowing firstRemainingRobberIndex to
    // occupy slot 2 - if either of the resulting slot assignments is possible, then so is this.
    //
    // This is a simple dynamic programming problem, and hopefully the code is self-explanatory :)
    //
    // The problem comes from the fact that the number of such slot assignments - and so, the size
    // of the lookup table - sounds like it should be pretty huge: each of remainingTimeSlot1 and remainingTimeSlot2
    // can be from 0 to g, and firstRemainingRobberIndex can be from 0 to n: since g can be up to 
    // 1'000'000, and n up to 100, this means that a naively-constructed lookup table could be of size
    // 1'000'000 x 1'000'000 * 100, which is vast :)
    //
    // Some optimisation is possible: g can be reduced to the maximum sum of all timeNeeded's without changing the 
    // answer, and this max sum (since timeNeeded <= 100) is at most 10'000.  Still pretty huge, though :)
    // Using a map to form a kind of "sparse lookup table" appears to drastically reduce this, but
    // I still can't shake off the suspicion that there should be some testcase that creates an enormous
    // number of SlotAssignment's.  Oh well :)
    //
    // And that's it for now!
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
