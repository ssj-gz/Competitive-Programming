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
    // Impose arbitrary ordering for normalisation of States.
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

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        cerr << "Expected: " << argv[0] << " <1|2>" << endl;
        return EXIT_FAILURE;
    }
    const int partNumber = stoi(argv[1]);
    assert((partNumber == 1) || (partNumber == 2));

    BoardScheme boardScheme;

    int width = -1;
    vector<string> configRows;
    string configRow;
    while (getline(cin, configRow))
    {
        configRows.push_back(configRow);
    }
    if (partNumber == 2)
    {
        configRows.insert(configRows.begin() + 3, "  #D#C#B#A#  ");
        configRows.insert(configRows.begin() + 4, "  #D#B#A#C#  ");
    }
    State initialState;
    int rowIndex = 0;
    for (const auto& configRow : configRows)
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

        cout << "# toExplore: " << toExplore.size() << " # states seen:" << bestCostForState.size() << " bestCostForEndState:" << bestCostForEndState << endl;

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
            for (const auto startCoord : state.coordsForAmphipodType[amphipodTypeIndex])
            {
                auto reachableCells = getReachableCells(startCoord, stateGrid);
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
                                    int highestAvailableRoomRow = -1;
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
                                        else
                                        {
                                            highestAvailableRoomRow = roomCoord.row;
                                        }
                                    }
                                    // Insist that we enter the "lowest" (i.e. highest available) empty cell in the room only:
                                    // this amphipod is in the room in needs to be in, so make room for the others!
                                    if (endCell.row != highestAvailableRoomRow)
                                        return true;
                                }
                                else
                                {
                                    if (boardScheme.amphidTypeForRoom(endCell) == boardScheme.amphidTypeForRoom(startCoord))
                                    {
                                        // Starting off in a room but ending up in the same room is a wasted move.
                                        return true;
                                    }
                                }
                            }

                            return false;

                            }
                ), reachableCells.end());
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
                        bestCostForState[nextState] = costToReachNextState;
                        previousState[nextState] = state;
                        toExplore.push_back({nextState});
                    }
                }

            }
        }
    }

    assert(bestCostForState.contains(endState));
    vector<State> statePath;
    auto currentState = endState;
    while (true)
    {
        statePath.push_back(currentState);
        if (!previousState.contains(currentState))
            break;
        currentState = previousState[currentState];
    }
    reverse(statePath.begin(), statePath.end());
    cout << "Backtracking:" << endl;
    for (const auto& state : statePath)
    {
        cout << bestCostForState[state] << endl;
        printState(state);
        cout << "---" << endl;
    }

    cout << bestCostForEndState << endl;

}
