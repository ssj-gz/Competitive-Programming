#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <limits>

#include <cassert>

using namespace std;

struct RegexNode
{
    enum Type { Start, End, LiteralDirections, Or };
    RegexNode(Type type)
        : type{type}
    {
        static int num = 0;
        num++;
        std::cout << "Created: " << num << " RegexNodes" << std::endl;

    }
    Type type;

    vector<std::pair<RegexNode*, RegexNode*>> choicesBeginAndEnd = {};
    string literalDirections = {};

    RegexNode *next = nullptr;

    void setNext(RegexNode* next)
    {
        if (type == Or)
        {
            for (auto [choiceBegin, choiceEnd] : choicesBeginAndEnd)
            {
                assert(choiceEnd);
                choiceEnd->setNext(next);
            }
        }
        else if (type == Start || type == LiteralDirections)
        {
            assert(!this->next);
            this->next = next;
        }
    }
};

struct Coord
{
    int x = -1;
    int y = -1;
    auto operator<=>(const Coord& coord) const = default;
};

struct State
{
    int x = -1;
    int y = -1;
    RegexNode *node = nullptr;
    auto operator<=>(const State& coord) const = default;
};

enum DirectionFlags
{
    North = 1,
    East = 2,
    South = 4,
    West = 8,
};

std::pair<RegexNode*,RegexNode*> parseRegexAux(const std::string& regexString, string indent)
{
    std::cout << indent << "Parsing: " << regexString << std::endl;
    if (regexString.empty())
    {
        RegexNode *node = new RegexNode{RegexNode::LiteralDirections};
        return {node, node};
    }

    int bracketLevel = 0;
    vector<string::size_type> topLevelPipePositions;
    bool hasTopLevelChars = 0;
    for (string::size_type pos = 0; pos < regexString.size(); pos++)
    {
        const char& character  = regexString[pos];
        if (character == '(')
            bracketLevel++;
        else if (character == ')')
            bracketLevel--;

        if (bracketLevel == 0)
        {
            if (pos != regexString.size() - 1 || character != ')')
                hasTopLevelChars = true;
            if (character == '|')
            {
                topLevelPipePositions.push_back(pos);
            }
        }
    }
    std::cout << indent << " #topLevelPipePositions: " << topLevelPipePositions.size() << " hasTopLevelChars: " << hasTopLevelChars << std::endl;
    if (!hasTopLevelChars)
    {
        assert(regexString.starts_with('('));
        assert(regexString.ends_with(')'));
        return parseRegexAux(regexString.substr(1, regexString.size() - 2), indent + " ");
    }
    if (!topLevelPipePositions.empty())
    {
        RegexNode *orNode = new RegexNode{RegexNode::Or};
        topLevelPipePositions.push_back(regexString.size() + 1); // Add a sentinel.
        string::size_type afterPrevPipePos = 0;
        for (const auto& pipePos : topLevelPipePositions)
        {
            const auto choiceSubExpr = regexString.substr(afterPrevPipePos, pipePos - afterPrevPipePos);
            std::cout << indent << "Flushing choiceSubExpr: " << choiceSubExpr << std::endl;
            orNode->choicesBeginAndEnd.push_back(parseRegexAux(choiceSubExpr, indent + " "));
            afterPrevPipePos = pipePos + 1;
        }
        return {orNode, orNode};
    }
    // Chain of top-level elements.
    bracketLevel = 0;
    string topLevelSubExpr;
    vector<RegexNode*> topLevelNodes;
    for (string::size_type pos = 0; pos < regexString.size(); pos++)
    {
        const char& character  = regexString[pos];
        if (character == '(')
        {
            if (bracketLevel == 0)
            {
                std::cout << indent << "flushing topLevelSubExpr: " << topLevelSubExpr << std::endl;
                topLevelNodes.push_back(parseRegexAux(topLevelSubExpr, indent + " ").first);
                topLevelSubExpr.clear();
            }
            topLevelSubExpr += "(";
            bracketLevel++;
        }
        else if (character == ')')
        {
            bracketLevel--;
            topLevelSubExpr += ")";
            if (bracketLevel == 0)
            {
                std::cout << indent << "flushing topLevelSubExpr: " << topLevelSubExpr << std::endl;
                topLevelNodes.push_back(parseRegexAux(topLevelSubExpr, indent + " ").first);
                topLevelSubExpr.clear();
            }
        }
        else
        {
            topLevelSubExpr += character;
        }
    }
    if (topLevelSubExpr == regexString)
    {
        RegexNode* directionsNode = new RegexNode{RegexNode::LiteralDirections};
        directionsNode->literalDirections = regexString;
        return {directionsNode, directionsNode};
    }
    if (!topLevelSubExpr.empty())
    {
        // Flush remaining topLevelSubExpr.
        std::cout << indent << "flushing topLevelSubExpr: " << topLevelSubExpr << std::endl;
        topLevelNodes.push_back(parseRegexAux(topLevelSubExpr, indent + " ").first);
    }
    RegexNode* previousTopLevelNode = nullptr;
    for (auto* topLevelNode : topLevelNodes)
    {
        if (previousTopLevelNode != nullptr)
            previousTopLevelNode->setNext(topLevelNode);
        previousTopLevelNode = topLevelNode;
    }
    assert(!topLevelNodes.empty());
    return {topLevelNodes.front(), topLevelNodes.back()};
}

void blah(RegexNode* node, string& directionsString, const int x, const int y, std::map<Coord, int>& availableDirectionsAt, std::set<State>& seenStates)
{
    State state = {x, y, node};
    if (seenStates.contains(state))
        return;
    seenStates.insert(state);
    std::cout << "# distinct states so far: " << seenStates.size() << std::endl;
    if (node->type == RegexNode::End)
    {
        static int numReachedEnd = 0;
        numReachedEnd++;
        std::cout << "Reached end (#" << numReachedEnd << "): " << x << ", " << y << " directionsString: " << directionsString << std::endl;
        //std::cout << "Reached end (#" << numReachedEnd << "): " << x << ", " << y << std::endl;
        //std::cout << "Reached end: " << x << ", " << y << std::endl;
        return;
    }
    else if (node->type == RegexNode::Start)
    {
        assert(node->next);
        blah(node->next, directionsString, 0, 0, availableDirectionsAt, seenStates);
    }
    if (node->type == RegexNode::LiteralDirections)
    {
        assert(node->next);
        const auto& literalDirections = node->literalDirections;
        const auto originalDirectionStringLen = directionsString.size();
        directionsString += literalDirections;

        int newX = x;
        int newY = y;
        for (const auto& directionChar : literalDirections)
        {
            switch (directionChar)
            {
                case 'N':
                    availableDirectionsAt[{newX, newY}] |= North;
                    newY--;
                    availableDirectionsAt[{newX, newY}] |= South;
                    break;
                case 'E':
                    availableDirectionsAt[{newX, newY}] |= East;
                    newX++;
                    availableDirectionsAt[{newX, newY}] |= West;
                    break;
                case 'S':
                    availableDirectionsAt[{newX, newY}] |= South;
                    newY++;
                    availableDirectionsAt[{newX, newY}] |= North;
                    break;
                case 'W':
                    availableDirectionsAt[{newX, newY}] |= West;
                    newX--;
                    availableDirectionsAt[{newX, newY}] |= East;
                    break;
                default:
                    assert(false);
            }
        }

        blah(node->next, directionsString, newX, newY, availableDirectionsAt, seenStates);

        directionsString.erase(directionsString.end() - literalDirections.size(), directionsString.end());
        assert(directionsString.size() == originalDirectionStringLen);
    }
    else if (node->type == RegexNode::Or)
    {
        assert(!node->next);
        for (auto [choiceBegin, choiceEnd] : node->choicesBeginAndEnd)
        {
            assert(choiceBegin);
            blah(choiceBegin, directionsString, x, y, availableDirectionsAt, seenStates);
        }
    }
} 

RegexNode* parseRegex(const std::string& regexString)
{
    assert(regexString.starts_with('^'));
    assert(regexString.ends_with('$'));
    RegexNode* startNode = new RegexNode{RegexNode::Start};
    RegexNode* endNode = new RegexNode{RegexNode::End};
    auto [bodyBeginNode, bodyEndNode] = parseRegexAux(regexString.substr(1, regexString.size() - 2), "");
    startNode->setNext(bodyBeginNode);
    bodyEndNode->setNext(endNode);
    return startNode;
}

int main()
{
    string regex;
    cin >> regex;
    std::cout << "regex: " << regex << std::endl;
    auto regexStartNode = parseRegex(regex);
    assert(regexStartNode->type == RegexNode::Start);
    string directionString;
    std::map<Coord, int> availableDirectionsAt;
    std::set<State> seenStates;
    blah(regexStartNode, directionString, 0, 0, availableDirectionsAt, seenStates);
    int minX = std::numeric_limits<int>::max();
    int minY = std::numeric_limits<int>::max();
    int maxX = std::numeric_limits<int>::min();
    int maxY = std::numeric_limits<int>::min();
    for (const auto& [coord, directionFlags] : availableDirectionsAt)
    {
        minX = std::min(minX, coord.x);
        minY = std::min(minY, coord.y);
        maxX = std::max(maxX, coord.x);
        maxY = std::max(maxY, coord.y);
    }
    const int gridWidth = 2 * (maxX - minX) + 3;
    const int gridHeight = 2 * (maxY - minY) + 3;
    vector<vector<char>> grid(gridWidth, vector<char>(gridHeight, '#'));
    for (const auto& [coord, directionFlags] : availableDirectionsAt)
    {
        const int gridX = (coord.x - minX) * 2 + 1;
        const int gridY = (coord.y - minY) * 2 + 1;
        assert(gridX - 1 >= 0 && gridX + 1 < gridWidth);
        assert(gridY - 1 >= 0 && gridY + 1 < gridHeight);
        grid[gridX][gridY] = '.';
        for (const auto directionFlag : { North, East, South, West })
        {
            if ((directionFlags & directionFlag) != 0)
            {
                switch (directionFlag)
                {
                    case North:
                        grid[gridX][gridY - 1] = '-';
                        break;
                    case East:
                        grid[gridX + 1][gridY] = '|';
                        break;
                    case South:
                        grid[gridX][gridY + 1] = '-';
                        break;
                    case West:
                        grid[gridX - 1][gridY] = '|';
                        break;
                }
            }

        }
        if (coord == Coord{0, 0})
            grid[gridX][gridY] = 'X';
    }

    for (int y = 0; y < gridHeight; y++)
    {
        for (int x = 0; x < gridWidth; x++)
        {
            std::cout << grid[x][y];
        }
        std::cout << std::endl;
    }

    set<Coord> toExplore = {{0, 0}};
    map<Coord, int> shortestPathTo;
    int doorsPassedThrough = 0;
    while (!toExplore.empty())
    {
        set<Coord> nextToExplore;
        for (const auto& coord : toExplore)
        {
            if (shortestPathTo.contains(coord))
                continue;
            shortestPathTo[coord] = doorsPassedThrough;
            std::cout << "shortest path to : " << coord.x << "," << coord.y << " is " << doorsPassedThrough << std::endl;
            const int directionFlags = availableDirectionsAt[coord];
            for (const auto directionFlag : { North, East, South, West })
            {
                if ((directionFlags & directionFlag) != 0)
                {
                    switch (directionFlag)
                    {
                        case North:
                            nextToExplore.insert({coord.x, coord.y - 1});
                            break;
                        case East:
                            nextToExplore.insert({coord.x + 1, coord.y});
                            break;
                        case South:
                            nextToExplore.insert({coord.x, coord.y + 1});
                            break;
                        case West:
                            nextToExplore.insert({coord.x - 1, coord.y});
                            break;
                    }
                }

            }
        }

        doorsPassedThrough++;
        toExplore = nextToExplore;
    }
    int largestDoorsPassedThrough = -1;
    for (const auto [coord, shortestPathNumDoors] : shortestPathTo)
        largestDoorsPassedThrough = std::max(largestDoorsPassedThrough, shortestPathNumDoors);
    std::cout << "largestDoorsPassedThrough: " << largestDoorsPassedThrough << std::endl;
    return 0;
}
