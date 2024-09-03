#include <iostream>
#include <vector>
#include <map>
#include <limits>

#include <cassert>

using namespace std;

enum Direction { Up, Right, Down, Left };
struct State
{
    int x = -1;
    int y = -1;
    Direction direction = Up;
    int numStepsInCurrentDirection = 0;
    auto operator<=>(const State& other) const = default;
    State afterMovingForward() const
    {
        State newState = *this;
        assert(numStepsInCurrentDirection < 10);
        switch (direction)
        {
            case Up:
                newState.y--;
                break;
            case Right:
                newState.x++;
                break;
            case Down:
                newState.y++;
                break;
            case Left:
                newState.x--;
                break;
        }
        newState.numStepsInCurrentDirection++;
        return newState;
    }
};

ostream& operator<<(ostream& os, const State& state)
{
    os << "State x: " << state.x << " y: " << state.y << " direction: ";
    char directionChar = '\0';
    switch (state.direction)
    {
        case Up:
            directionChar = '^';
            break;
        case Right:
            directionChar = '>';
            break;
        case Down:
            directionChar = 'v';
            break;
        case Left:
            directionChar = '<';
            break;
    }
    os << directionChar << " numStepsInCurrentDirection: " << state.numStepsInCurrentDirection << std::endl;

    return os;
}

int main()
{
    vector<string> rawMap;
    string rawMapRow;
    while (getline(cin, rawMapRow))
    {
        rawMap.push_back(rawMapRow);
    }

    const int width = rawMap.front().size();
    const int height = rawMap.size();

    vector<vector<int>> heatLossMap(width, vector<int>(height, -1));
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            heatLossMap[x][y] = rawMap[y][x] - '0';
        }
    }


    vector<State> toExplore;
    map<State, int> lowestHeatLossForState;
    map<State, State> predecessorState;
    for (auto direction : { Up, Right, Down, Left })
    {
        State startingState = { 0, 0, direction, 0 };
        toExplore.push_back(startingState);
        lowestHeatLossForState[startingState] = 0; // No initial heat loss.
    }
    while (!toExplore.empty())
    {
        std::cout << "#toExplore: " << toExplore.size() << std::endl;
        vector<State> nextToExplore;
        for (const auto& state : toExplore)
        {
            vector<State> successorStates;
            const int heatLoss = lowestHeatLossForState[state];
            if (state.numStepsInCurrentDirection != 10)
            {
                successorStates.push_back(state.afterMovingForward());
            }
            if (state.numStepsInCurrentDirection >= 4)
            {
                for (auto turnDir : { -1, +1})
                {
                    const Direction otherDirection = static_cast<Direction>((static_cast<int>(state.direction) + 4 + turnDir) % 4);
                    State moveInNewDirection = { state.x, state.y, otherDirection, 0 };
                    moveInNewDirection = moveInNewDirection.afterMovingForward();
                    successorStates.push_back(moveInNewDirection);
                }
            }

            for (const auto& successorState : successorStates)
            {
                if (successorState.x < 0 || successorState.x >= width || successorState.y < 0 || successorState.y >= height)
                    continue;
                assert(successorState.numStepsInCurrentDirection <= 10);
                const int heatLossForSuccessorState = heatLoss + heatLossMap[successorState.x][successorState.y];
                if (!lowestHeatLossForState.contains(successorState) || heatLossForSuccessorState < lowestHeatLossForState[successorState] )
                {
                    lowestHeatLossForState[successorState] = heatLossForSuccessorState;
                    nextToExplore.push_back(successorState);
                    predecessorState[successorState] = state;
                }
            }

        }
        toExplore = nextToExplore;
    }

    int minHeatLoss = std::numeric_limits<int>::max();
    State bestEndState;
    for (const auto& [state, heatLoss] : lowestHeatLossForState)
    {
        if (state.x == width - 1 && state.y == height - 1 && state.numStepsInCurrentDirection >= 4)
        {
            if (heatLoss < minHeatLoss)
            {
                minHeatLoss = heatLoss;
                bestEndState = state;
            }
        }
    }
#if 1
    vector<vector<char>> finalPathMap(width, vector<char>(height, '.'));
    State currentState = bestEndState;
    std::cout << "backtracking: " << std::endl;
    int blah = 0;
    do
    {
        std::cout << " currentState: " << currentState << std::endl;
        char directionChar = '\0';
        switch (currentState.direction)
        {
            case Up:
                directionChar = '^';
                break;
            case Right:
                directionChar = '>';
                break;
            case Down:
                directionChar = 'v';
                break;
            case Left:
                directionChar = '<';
                break;
        }
        finalPathMap[currentState.x][currentState.y] = directionChar;
        if (!predecessorState.contains(currentState))
            break;
        blah += heatLossMap[currentState.x][currentState.y];
        currentState = predecessorState[currentState];
    }
    while (true);
    std::cout << "blah: " << blah << std::endl;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            std::cout << finalPathMap[x][y];
        }
        std::cout << std::endl;
    }
#endif
    std::cout << "minHeatLoss: " << minHeatLoss << std::endl;

}
