#include <iostream>
#include <vector>
#include <deque>
#include <map>
#include <set>
#include <algorithm>
#include <limits>
#include <cassert>

using namespace std;

enum Type
{
    Space,
    Hall,
    Wall,
    RoomA,
    RoomB,
    RoomC,
    RoomD
};

enum AmphipodType
{
    A,
    B,
    C,
    D
};

const AmphipodType amphiPodTypes[] = { A, B, C, D};
const int moveCostForAmphipod[] = {1, 10, 100, 1000};
constexpr int numAmphiPodTypes = static_cast<int>(std::size(amphiPodTypes));

struct Coord
{
    int row = -1;
    int col = -1;
    // Impose arbitrary ordering.
    auto operator<=>(const Coord& other) const = default;
};

ostream& operator<<(ostream& os, const Coord& coord)
{
    os << "(" << coord.row << "," << coord.col << ")";
    return os;
}

struct BoardScheme
{
    vector<vector<Type>> scheme;

    bool isRoom(Coord coord) const
    {
        switch(scheme[coord.row][coord.col])
        {
        case RoomA:
        case RoomB:
        case RoomC:
        case RoomD:
            return true;
        default:
            return false;
        }
    }
    AmphipodType amphidTypeForRoom(const Coord roomCoord)
    {
        assert(isRoom(roomCoord));
        switch(scheme[roomCoord.row][roomCoord.col])
        {
        case RoomA:
            return A;
        case RoomB:
            return B;
        case RoomC:
            return C;
        case RoomD:
            return D;
        default:
            assert(false);
            return A;
        }
    }
    vector<Coord> roomCoordsForAmphidType[numAmphiPodTypes];
};

struct State
{
    vector<Coord> coordsForAmphipodType[numAmphiPodTypes] = {};
    // Impose arbitrary ordering for use in maps/ sets etc.
    auto operator<=>(const State& other) const = default;
    void normalise()
    {
        for (auto& coords : coordsForAmphipodType)
        {
            sort(coords.begin(), coords.end());
        }
    }
};

int main()
{
    BoardScheme boardScheme;

    int width = -1;
    string configRow;
    State initialState;
    int rowIndex = 0;
    while (getline(cin, configRow))
    {
        assert(width == -1 || static_cast<int>(configRow.size()) == width);
        width = static_cast<int>(configRow.size());

        vector<Type> schemeRow;
        int roomTypeIndex = 0;
        int colIndex = 0;
        for (const auto letter : configRow)
        {
            if (letter == ' ')
                schemeRow.push_back(Space);
            else if (letter == '.')
                schemeRow.push_back(Hall);
            else if (letter == '#')
                schemeRow.push_back(Wall);
            else 
            {
                assert(letter >= 'A' && letter <= 'D');

                schemeRow.push_back(static_cast<Type>(RoomA + roomTypeIndex));
                boardScheme.roomCoordsForAmphidType[roomTypeIndex].push_back({rowIndex, colIndex});
                roomTypeIndex++;

                initialState.coordsForAmphipodType[A + (letter - 'A')].push_back({rowIndex, colIndex});


            }
            colIndex++;
        }
        boardScheme.scheme.push_back(schemeRow);
        rowIndex++;
    }
    const int height = static_cast<int>(boardScheme.scheme.size());
    cout << "width: " << width << " height: " << height << endl;
    cout << "boardScheme:" << endl;
    for (const auto& schemeRow : boardScheme.scheme)
    {
        for (const auto schemeCell : schemeRow)
        {
            switch (schemeCell)
            {
                case Space:
                    cout << ' ';
                    break;
                case Hall:
                    cout << 'H';
                    break;
                case Wall:
                    cout << '#';
                    break;
                case RoomA:
                    cout << 'a';
                    break;
                case RoomB:
                    cout << 'b';
                    break;
                case RoomC:
                    cout << 'c';
                    break;
                case RoomD:
                    cout << 'd';
                    break;
            }
        }
        cout  << endl;
    }

    auto stateAsGrid = [&](const State& state)
    {
        vector<string> output(height, string(width, ' '));
        for (int rowIndex = 0; rowIndex < height; rowIndex++)
        {
            for (int colIndex = 0; colIndex < width; colIndex++)
            {
                switch (boardScheme.scheme[rowIndex][colIndex])
                {
                    case Space:
                        output[rowIndex][colIndex] = ' ';
                        break;
                    case Wall:
                        output[rowIndex][colIndex] = '#';
                        break;
                    default:
                        output[rowIndex][colIndex] = '.';
                }
                for (int amphipodTypeIndex = 0; amphipodTypeIndex < numAmphiPodTypes; amphipodTypeIndex++)
                {
                    for (const auto& coord : state.coordsForAmphipodType[amphipodTypeIndex])
                    {
                        output[coord.row][coord.col] = static_cast<char>('A' + amphipodTypeIndex);
                    }
                }
            }
        }
        return output;
    };

    auto printState = [&](const State& state)
    {
        for (const auto& row : stateAsGrid(state))
        {
            cout << row << endl;
        }
    };

    printState(initialState);

    deque<State> toExplore = { initialState };
    map<State, int> bestCostForState;
    bestCostForState[initialState] = 0;
    map<State, State> previousState;

    State endState;
    for (int amphipodTypeIndex = 0; amphipodTypeIndex < numAmphiPodTypes; amphipodTypeIndex++)
    {
        for (const auto roomCoord : boardScheme.roomCoordsForAmphidType[amphipodTypeIndex])
        {
            endState.coordsForAmphipodType[amphipodTypeIndex].push_back(roomCoord);
        }
    }
    endState.normalise();
    cout << "End state:" << endl;
    for (const auto& row : stateAsGrid(endState))
    {
        cout << row << endl;
    }

    struct ReachableCell
    {
        Coord cell;
        int numStepsToReach = -1;
    };

    auto getReachableCells = [&](const Coord initialPos, const vector<string>& stateAsGrid)
    {
        vector<ReachableCell> result;
        vector<string> stateGridCopy(stateAsGrid);
        deque<ReachableCell> toExplore = { {initialPos, 0 } };
        while (!toExplore.empty())
        {
            const auto [cell, numStepsToReach] = toExplore.front();
            toExplore.pop_front();

            const pair<int, int> dxdy[] =
            {
                {-1, 0},
                {+1, 0},
                {0, -1},
                {0, +1}
            };

            for (const auto& offset : dxdy)
            {
                const int neighbourRow = cell.row - offset.first;
                const int neighbourCol = cell.col - offset.second;
                if (neighbourRow < 0 || neighbourRow >= height)
                    continue;
                if (neighbourCol < 0 || neighbourCol >= width)
                    continue;
                if (stateGridCopy[neighbourRow][neighbourCol] != '.')
                    continue;

                result.push_back({{neighbourRow, neighbourCol}, numStepsToReach + 1});
                stateGridCopy[neighbourRow][neighbourCol] = 'X';
                toExplore.push_back({{neighbourRow, neighbourCol}, numStepsToReach + 1});
            }
        }
        return result;
    };

    int bestCostForEndState = numeric_limits<int>::max();
    while (!toExplore.empty())
    {
        const auto state = toExplore.front();
        toExplore.pop_front();

        //cout << "State:" << endl;
        //printState(state);
        cout << "# toExplore: " << toExplore.size() << " # states seen:" << bestCostForState.size() << endl;

        if (state == endState)
        {
            if (bestCostForState[endState] < bestCostForEndState)
            {
                cout << "Woo!" << bestCostForState[endState] << endl;
                bestCostForEndState = bestCostForState[endState];
            }
        }


        const auto stateGrid = stateAsGrid(state);
        for (int amphipodTypeIndex = 0; amphipodTypeIndex < numAmphiPodTypes; amphipodTypeIndex++)
        {
            const char ampidLetter = static_cast<char>('A' + amphipodTypeIndex);
            for (const auto startCoord : state.coordsForAmphipodType[amphipodTypeIndex])
            {
                //cout << "Moving " << ampidLetter << " starting at " << startCoord << " initial reachable cells:" << endl;
                auto reachableCells = getReachableCells(startCoord, stateGrid);
#if 0
                auto reachableStateGrid = stateGrid;
                for (const auto [cell, numStepsToReach] : reachableCells)
                {
                    reachableStateGrid[cell.row][cell.col] = static_cast<char>('0' + numStepsToReach);
                }
                for (const auto& row : reachableStateGrid)
                {
                    cout << row << endl;
                }
#endif
                reachableCells.erase(remove_if(reachableCells.begin(), reachableCells.end(),
                            [&](const ReachableCell& reachableCell)
                            {
                                const auto endCell = reachableCell.cell;
                                const auto endType = boardScheme.scheme[endCell.row][endCell.col];
                                if (endType == Hall &&
                                    boardScheme.isRoom(Coord{endCell.row + 1, endCell.col}))
                                {
                                    // Don't stop just outside any rooms.
                                    return true;
                                }

                                if (boardScheme.scheme[startCoord.row][startCoord.col] == Hall &&
                                        !boardScheme.isRoom(endCell))
                                {
                                    // If we start off in the hall, we can only end up in a room.
                                    return true;
                                }

                                if (boardScheme.isRoom(endCell))
                                {
                                    const auto amphidTypeForRoom = boardScheme.amphidTypeForRoom(endCell);
                                    const bool enteredRoom = (boardScheme.scheme[startCoord.row][startCoord.col] == Hall) 
                                    || amphidTypeForRoom != boardScheme.amphidTypeForRoom(startCoord);
                                    if (enteredRoom)
                                    {
                                        // Amphipods of this type cannot enter this particular room.
                                        if (boardScheme.amphidTypeForRoom(endCell) != amphiPodTypes[amphipodTypeIndex])
                                            return true;

                                        // Allowed to enter room type - but is there an "alien" Amphipod in there already?
                                        for (const auto roomCoord : boardScheme.roomCoordsForAmphidType[amphidTypeForRoom])
                                        {
                                            const auto letterInRoom = stateGrid[roomCoord.row][roomCoord.col];
                                            if (letterInRoom != '.')
                                            {
                                                assert(letterInRoom >= 'A' && letterInRoom <= 'D');
                                                if (A + (letterInRoom - 'A') != amphidTypeForRoom)
                                                {
                                                    return true;
                                                }

                                            }
                                        }
                                    }
                                }

                                return false;

                            }
                            ), reachableCells.end());
#if 0
                cout << "After filtering:" << endl;
                reachableStateGrid = stateGrid;
                for (const auto [cell, numStepsToReach] : reachableCells)
                {
                    reachableStateGrid[cell.row][cell.col] = static_cast<char>('0' + numStepsToReach);
                }
                for (const auto& row : reachableStateGrid)
                {
                    cout << row << endl;
                }
#endif
                for (const auto reachableCell : reachableCells)
                {
                    State nextState = state;
                    auto& coordsForAmphidType = nextState.coordsForAmphipodType[amphipodTypeIndex];
                    coordsForAmphidType.erase(remove(coordsForAmphidType.begin(), coordsForAmphidType.end(), startCoord), coordsForAmphidType.end());
                    coordsForAmphidType.push_back(reachableCell.cell);
                    nextState.normalise();

                    const int costToReachNextState = bestCostForState[state] + reachableCell.numStepsToReach * moveCostForAmphipod[amphipodTypeIndex];

                    if (!bestCostForState.contains(nextState) || costToReachNextState < bestCostForState[nextState])
                    {
#if 0
                        cout << "Next state:" << endl;
                        for (const auto& row : stateAsGrid(nextState))
                        {
                            cout << row << endl;
                        }
                        if (!bestCostForState.contains(nextState))
                        {
                            cout << "Brand new state!" << endl;
                        }
                        else
                        {
                            cout << "Cheaper state: was " << bestCostForState[nextState] << " now " << costToReachNextState << endl; 
                        }
#endif
                        bestCostForState[nextState] = costToReachNextState;
                        previousState[nextState] = state;
                        toExplore.push_back({nextState});
                    }
                }

            }
        }
    }

    assert(bestCostForState.contains(endState));
    vector<State> blah;
    auto currentState = endState;
    while (true)
    {
        blah.push_back(currentState);
        if (!previousState.contains(currentState))
            break;
        currentState = previousState[currentState];
    }
    reverse(blah.begin(), blah.end());
    cout << "Backtracking:" << endl;
    for (const auto& state : blah)
    {
        cout << bestCostForState[state] << endl;
        printState(state);
        cout << "---" << endl;
    }


}
