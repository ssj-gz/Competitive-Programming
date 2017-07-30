#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <cassert>

using namespace std;

int numDisks;
constexpr int numPoles = 4;

class HanoiState
{
    public:
        void addToPole(int poleIndex, int disc)
        {
            pole[poleIndex] |= (1 << disc);
        }
        int topDisk(int poleIndex) const
        {
            if (pole[poleIndex] == 0)
                return -1;

            static const vector<int> lowestOnBitTable = computeLowestOnBitTable();

            const int disk = lowestOnBitTable[pole[poleIndex]]
                - 1; // Discs are 0-relative.

            return disk;
        };
        void move(int sourcePoleIndex, int destPoleIndex)
        {
            const int discToMove = popDiscFromPole(sourcePoleIndex);
            addToPole(destPoleIndex, discToMove);
        }

        bool isGoalState() const
        {
            for (int poleIndex = 1; poleIndex < numPoles; poleIndex++)
            {
                if (pole[poleIndex] != 0)
                    return false;
            }
            return true;
        }
        int uniqueId() const
        {
            // (pole of disc 0) * 4^0 + (pole of disc 1) * 4^1 + ... (pole of disc numDisks - 1) * 4^(numDisks - 1)
            const static vector<int> powersOfNumPoles = computePowersOfNumPoles();
            int hash = 0;
            for (int poleIndex = 0; poleIndex < numPoles; poleIndex++)
            {
                const int poleContents = pole[poleIndex];
                for (int discIndex = 0; discIndex < numDisks; discIndex++)
                {
                    if (poleContents & (1 << discIndex))
                    {
                        hash += poleIndex * powersOfNumPoles[discIndex];
                    }
                }
            }
            return hash;
        }
    private:
        // Contents of poles: if disc i (0-relative) is
        // on pole p (again, 0-relative), then (p & (1 << i)) != 0.
        int pole[numPoles] = {0};
        int popDiscFromPole(int poleIndex)
        {
            const int disk = topDisk(poleIndex);
            assert((pole[poleIndex] & (1 << disk)) != 0);
            pole[poleIndex] -= (1 << disk);
            return disk;
        }
        static vector<int> computePowersOfNumPoles()
        {
            int power = 1;
            vector<int> powers;
            for (int i = 0; i < numDisks; i++)
            {
                powers.push_back(power);
                power *= numPoles;
            }
            return powers;
        }
        static vector<int> computeLowestOnBitTable()
        {
            vector<int> lowestOnBitTable;
            const int highestValue = (1 << (numDisks + 1)) - 1;
            for (int i = 0; i < highestValue; i++)
            {
                int value = i;
                if (value == 0)
                {
                    lowestOnBitTable.push_back(-1);
                    continue;
                }
                int lowestOnBit = 1;
                while (value != 0)
                {
                    if ((value & 1) == 1)
                    {
                        lowestOnBitTable.push_back(lowestOnBit);
                        break;
                    }
                    value >>= 1;
                    lowestOnBit++;
                }
            }
            return lowestOnBitTable;
        }
};


int main()
{
    cin >> numDisks;
    HanoiState initialState;
    for (int discIndex = 0; discIndex < numDisks; discIndex++)
    {
        int poleIndex;
        cin >> poleIndex;
        // We're working with zero-relative disc and pole indices.
        poleIndex--;

        initialState.addToPole(poleIndex, discIndex);
    }

    // A straightforward breadth-first search will suffice, though we need
    // to optimise HanoiState for size and speed of operations - no using 
    // "std::stack" for pole contents!
    // There are plenty of additional optimisations that we could add, though:
    // firstly, note that given a state S, the number of moves to get from S
    // to the goal state is the same as that from a state S', where S' is formed
    // from S by permuting any pair of the poles not including the first:
    // that is, from the point of view of calculating the number of moves to the 
    // goal state, we can impose an equivalence relation R on the set of states 
    // defined by (S,S') in R if and only if S can be transformed into S' by permuting
    // its 2nd, 3rd and 4th poles. We can then just deal with "normalised" states,
    // where we pick some arbitrary representative of the equivalence class containing
    // each state: perhaps one where the 2nd, 3rd and 4th poles are arranged so that their
    // topmost discs are in sorted order.  This would likely shrink the number of states
    // by quite a lot.
    // Additionally, we can ensure that if we moved a disk last move, then we don't move that
    // same disk this move: it would render the previous move pointless.
    // More speculatively, if the last move moved a disc from pS to pD, perhaps we could 
    // insist that the next move moves to or from one of pS or pD - not sure if this would work,
    // though.
    set<int> allSeenStateIds;
    list<HanoiState> statesToProcess;

    allSeenStateIds.insert(initialState.uniqueId());
    statesToProcess.push_back(initialState);

    int numMoves = 1;
    while (!statesToProcess.empty())
    {
        list<HanoiState> newStatesToProcess;
        for (const auto& state : statesToProcess)
        {
            for (int sourcePoleIndex = 0; sourcePoleIndex < numPoles; sourcePoleIndex++)
            {
                for (int destPoleIndex = 0; destPoleIndex < numPoles; destPoleIndex++)
                {
                    if (sourcePoleIndex == destPoleIndex)
                        continue;
                    if (state.topDisk(sourcePoleIndex) == -1)
                        continue;
                    if (state.topDisk(destPoleIndex) != -1 && state.topDisk(destPoleIndex) < state.topDisk(sourcePoleIndex))
                        continue;

                    HanoiState newState(state);
                    newState.move(sourcePoleIndex, destPoleIndex);

                    if (newState.isGoalState())
                    {
                        cout << numMoves;
                        return 0;
                    }

                    const int newStateHash = newState.uniqueId();
                    if (allSeenStateIds.find(newStateHash) != allSeenStateIds.end())
                        continue;

                    newStatesToProcess.push_back(newState);
                    allSeenStateIds.insert(newStateHash);
                }
            }
        }
        statesToProcess = newStatesToProcess;
        numMoves++;
    }
    assert(false); // Whoops

}


