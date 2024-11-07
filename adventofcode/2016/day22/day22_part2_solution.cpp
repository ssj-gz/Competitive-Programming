#include <iostream>
#include <regex>
#include <set>
#include <optional>

#include <cassert>

using namespace std;

struct Coord
{
    int x = -1;
    int y = -1;
    auto operator<=>(const Coord& other) const = default;
};

struct Node
{
    Coord coord;
    int used = -1;
    int avail = -1;
    int size = -1;
    auto operator<=>(const Node& other) const = default;
};

int maxXForY0 = -1;
int maxX = -1;
int maxY = -1;


struct State
{
    const vector<vector<Node>>* initialNodeGrid = nullptr;
    Coord targetDataCoord;

    map<Coord, Node> nodeGridOverride = {};
    Node& nodeAt(const Coord& coord)
    {
        if (!nodeGridOverride.contains(coord))
        {
            nodeGridOverride[coord] = (*initialNodeGrid)[coord.x][coord.y];
        }
        return nodeGridOverride[coord];
    }
    const Node& nodeAt(const Coord& coord) const
    {
        if (nodeGridOverride.contains(coord))
            return nodeGridOverride.find(coord)->second;
        else
            return (*initialNodeGrid)[coord.x][coord.y];
    }
    void discard()
    {
        auto overrideIter = nodeGridOverride.begin();
        while (overrideIter != nodeGridOverride.end())
        {
            if (overrideIter->second == (*initialNodeGrid)[overrideIter->second.coord.x][overrideIter->second.coord.y])
                overrideIter = nodeGridOverride.erase(overrideIter);
            else
                overrideIter++;
        }
    }
    bool canDiscard() const
    {
        for (const auto& [coord, node] : nodeGridOverride)
        {
            if (node == (*initialNodeGrid)[coord.x][coord.y])
                return true;
        }
        return false;
    }
    auto operator<=>(const State& other) const = default;
};

int main()
{
    // root@ebhq-gridcenter# df -h
    string dfCommand;
    getline(cin, dfCommand);
    // Filesystem              Size  Used  Avail  Use%
    string headers;
    getline(cin, headers);


    std::regex dfRegex(R"(^/dev/grid/node-x(\d+)-y(\d+)\s*(\d+)T\s*(\d+)T\s*(\d+)T\s*(\d+)%$)");
    string dfLine;
    vector<Node> nodes;
    int64_t totalUsed = 0;
    int64_t totalSize = 0;
    while (getline(cin, dfLine))
    {
        std::smatch dfMatch;
        const bool matchSuccessful = std::regex_match(dfLine, dfMatch, dfRegex);
        assert(matchSuccessful);
        Node node;
        node.coord.x = std::stoll(dfMatch[1]);
        node.coord.y = std::stoll(dfMatch[2]);
        node.size = std::stoll(dfMatch[3]);
        assert(node.size > 0);
        totalSize += node.size;
        node.used = std::stoll(dfMatch[4]);
        totalUsed += node.used;
        node.avail = std::stoll(dfMatch[5]);
        nodes.push_back(node);
        if (node.coord.y == 0)
            maxXForY0 = std::max(maxXForY0, node.coord.x);
        maxX = std::max(maxX, node.coord.x);
        maxY = std::max(maxY, node.coord.y);
    }
    std::cout << "maxXForY0: " << maxXForY0 << std::endl;
    vector<vector<Node>> initialNodeGrid(maxX + 1, vector<Node>(maxY + 1));
    for (auto& node : nodes)
    {
        initialNodeGrid[node.coord.x][node.coord.y] = node;
    }
    const auto initialNodeGridBak = initialNodeGrid;

    for (int x = 0; x <= maxX; x++)
    {
        for (int y = 0; y <= maxY; y++)
        {
             auto& cell = initialNodeGrid[x][y];

            const bool canNeverMoveToNeighbour = cell.used >= 490; // TODO - don't hardcode this!
            if (canNeverMoveToNeighbour)
            {
                cell.used = 2;
                cell.avail = 0;
                std::cout << "x: " << x << " y: " << y << " can never move to neighbour" << std::endl;
            }
            else
            {
                if (cell.used > 0)
                    cell.used = 1;
                cell.avail = (cell.used ? 0 : 1);
            }
        }
    }

    State initialState = { &initialNodeGrid, {maxXForY0, 0} };
    //initialState.normalise();
#if 1
    vector<State> toExplore = { initialState };
    set<State> seen = { initialState };

    set<Coord> active;
    int numSteps = 0;
    std::optional<State> finalState;
    map<State, State> predecessorOfState;
    while (!toExplore.empty() && !finalState.has_value())
    {
        std::cout << "numSteps: " << numSteps << " #toExplore: " << toExplore.size() << std::endl;
        vector<State> nextToExplore;
        for (const auto& state : toExplore)
        {
            if (state.targetDataCoord == Coord{0, 0})
            {
                std::cout << "woo!" << std::endl;
                finalState = state;
                break;
            }

            auto calcActive = [](const auto& state)
            {
                set<Coord> active;
                for (int x = 0; x <= maxX; x++)
                {
                    for (int y = 0; y <= maxY; y++)
                    {
                        const auto& node = state.nodeAt({x, y});
                        const auto amountOfDataToMove = node.used;
                        for (const auto& [dx, dy] : std::initializer_list<std::pair<int, int>>{ {-1, 0},
                                {+1, 0},
                                {0, -1},
                                {0, +1}
                                })
                        {
                            const int neighbourX = x + dx;
                            const int neighbourY = y + dy;
                            if (neighbourX < 0 || neighbourX > maxX || neighbourY < 0 || neighbourY > maxY)
                                continue;
                            const auto& neighbourNode = state.nodeAt({neighbourX, neighbourY});
                            if (amountOfDataToMove <= neighbourNode.avail)
                                active.insert({x, y});
                        }
                    }
                }
                return active;

            };
            auto calcFree = [](const auto& state)
            {
                int free = 0;
                for (int x = 0; x <= maxX; x++)
                {
                    for (int y = 0; y <= maxY; y++)
                    {
                        const auto& node = state.nodeAt({x, y});
                        if (node.used == 0)
                            free++;
                    }
                }
                return free;

            };

            //std::cout << "num active: "<< calcActive(state).size() << std::endl;
            //std::cout << "num free: "<< calcFree(state) << std::endl;
            assert(calcFree(state) == 1);
            //const int origMobility = calcMobility(state);
            //std::cout << "origMobility: " << origMobility << std::endl;
            for (int x = 0; x <= maxX; x++)
            {
                for (int y = 0; y <= maxY; y++)
                {
                    const auto& node = state.nodeAt({x, y});
                    const auto amountOfDataToMove = node.used;
                    for (const auto& [dx, dy] : std::initializer_list<std::pair<int, int>>{ {-1, 0},
                            {+1, 0},
                            {0, -1},
                            {0, +1}
                            })
                    {
                        const int neighbourX = x + dx;
                        const int neighbourY = y + dy;
                        if (neighbourX < 0 || neighbourX > maxX || neighbourY < 0 || neighbourY > maxY)
                            continue;
                        const auto& neighbourNode = state.nodeAt({neighbourX, neighbourY});
                        if (amountOfDataToMove <= neighbourNode.avail)
                        {
                            bool movesFromTarget = (x == state.targetDataCoord.x && y == state.targetDataCoord.y);
                            if (movesFromTarget)
                            {
                                //std::cout << "moving from target: " << state.targetDataCoord.x << ", " << state.targetDataCoord.y << " to " << neighbourX << ", " << neighbourY << std::endl;
                                //active.clear();
                            }
                            State nextState = state;
                            auto& from = nextState.nodeAt({x, y});
                            auto& to = nextState.nodeAt({neighbourX, neighbourY});
                            assert(from.used == amountOfDataToMove);
                            assert(to.avail >= amountOfDataToMove);

                            to.avail -= amountOfDataToMove;
                            to.used += amountOfDataToMove;

                            from.avail += amountOfDataToMove;
                            from.used = 0;

                            if (movesFromTarget)
                            {
                                nextState.targetDataCoord = { neighbourX, neighbourY };
                            }

                            nextState.discard();

#if 0
                            bool hasNew = false;
                            for (const auto& blah : calcActive(nextState))
                            {
                                if (!active.contains(blah))
                                {
                                    hasNew = true;
                                    active.insert(blah);
                                    //std::cout << "New active: " << blah.x << ", "  << blah.y << std::endl;
                                }
                            }
                            //if (!hasNew && !movesFromTarget)
                                //continue;
#endif


                            //nextState.normalise();
                            if (!seen.contains(nextState))
                            {
                                seen.insert(nextState);
                                predecessorOfState[nextState] = state;
                                nextToExplore.push_back(nextState);
                            }
                        }
                    }
                }
            }
        }
        toExplore = nextToExplore;
        numSteps++;
    }
#endif
    assert(finalState.has_value());
    State currentState = finalState.value();
    vector<std::pair<Coord, Coord>> moves;
    while (true)
    {
        if (predecessorOfState.contains(currentState))
        {
            const auto& predecessor = predecessorOfState[currentState];
            std::optional<Coord> from;
            std::optional<Coord> to;
            for (int x = 0; x <= maxX; x++)
            {
                for (int y = 0; y <= maxY; y++)
                {
                    auto& currentCell = currentState.nodeAt({x,y});
                    auto& precessorCell = predecessor.nodeAt({x,y});
                    if (currentCell.used == 0 && precessorCell.used != 0)
                    {
                        assert(!from.has_value());
                        from = {x, y};
                    }
                    if (currentCell.used != 0 && precessorCell.used == 0)
                    {
                        assert(!to.has_value());
                        to = {x, y};
                    }
                }
            }

            assert(from.has_value());
            assert(to.has_value());
            moves.push_back({from.value(), to.value()});
            
            currentState = predecessor;
        }
        else
        {
            assert(currentState == initialState);
            break;
        }

    }
    std::reverse(moves.begin(), moves.end());
    auto nodeGrid = initialNodeGridBak;
    for (const auto& move : moves)
    {
        const auto [fromX, fromY] = move.first;
        const auto [toX, toY] = move.second;
        std::cout << "move from: " << fromX << ", " << fromY << " to " << toX << ", " << toY << std::endl;
        assert(nodeGrid[fromX][fromY].used > 0);
        assert(nodeGrid[fromX][fromY].used <= nodeGrid[toX][toY].avail);
        const auto amountOfDataToMove = nodeGrid[fromX][fromY].used;
        nodeGrid[toX][toY].avail -= amountOfDataToMove;
        nodeGrid[toX][toY].used += amountOfDataToMove;
        nodeGrid[fromX][fromY].used = 0;
        nodeGrid[fromX][fromY].avail += amountOfDataToMove;
    }

    
#if 0
    const Coord coordToClear = { maxXForY0, 0 };
    set<Coord> processing = { };
    makeSpace(coordToClear, initialNodeGrid[coordToClear.x][coordToClear.y].used, processing, initialState);
#endif


    return 0;
}
