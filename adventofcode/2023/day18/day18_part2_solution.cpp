#include <iostream>
#include <vector>
#include <regex>
#include <map>
#include <set>
#include <limits>
#include <fstream>
#include <optional>
#include <ranges>

#include <cassert>

using namespace std;

struct Coord
{
    int64_t x = -1;
    int64_t y = -1;
    auto operator<=>(const Coord& other) const = default;
};

enum Direction { None, Up, Left, Down, Right };

struct DigSection
{
    Direction direction = Up;
    int64_t numToDig = -1;
    string colourRGB;
};

#ifdef BRUTE_FORCE
int64_t lagoonSizeBruteForce(const vector<DigSection>& digPlan)
{
    std::map<Coord, string> colourAtCoord;
    int64_t x = 0;
    int64_t y = 0;

    for (const auto& digSection : digPlan)
    {
        for (int64_t i = 0; i < digSection.numToDig; i++)
        {
            switch (digSection.direction)
            {
                case Up:
                    y--;
                    break;
                case Right:
                    x++;
                    break;
                case Down:
                    y++;
                    break;
                case Left:
                    x--;
                    break;
            }
            colourAtCoord[{x, y}] = digSection.colourRGB;
        }
    }

    int64_t minX = std::numeric_limits<int64_t>::max();
    int64_t minY = std::numeric_limits<int64_t>::max();
    int64_t maxX = std::numeric_limits<int64_t>::min();
    int64_t maxY = std::numeric_limits<int64_t>::min();
    for (const auto& [coord, colour] : colourAtCoord)
    {
        minX = std::min(minX, coord.x);
        minY = std::min(minY, coord.y);
        maxX = std::max(maxX, coord.x);
        maxY = std::max(maxY, coord.y);
    }
    const int64_t width = maxX - minX + 3;
    const int64_t height = maxY - minY + 3;
    assert(width > 0);
    assert(height > 0);
    vector<vector<char>> fillMap(width, vector<char>(height, '.'));
    for (const auto& [coord, colour] : colourAtCoord)
    {
        const int64_t x = coord.x - minX + 1;
        const int64_t y = coord.y - minY + 1;
        assert(x >= 0 && x < width);
        assert(y >= 0 && y < height);
        fillMap[x][y] = '#';
    }
    for (int64_t y = 0; y < height; y++)
    {
        for (int64_t x = 0; x < width; x++)
        {
            cout << fillMap[x][y];
        }
        cout << endl;
    }

    vector<Coord> toExplore = { {0, 0} };
    set<Coord> seen = { {0, 0} };

    int64_t numOutsideCells = 0;
    while (!toExplore.empty())
    {
        std::cout << "#toExplore: " << toExplore.size() << std::endl;
        vector<Coord> nextToExplore;

        for (const auto& coord : toExplore)
        {
            assert(fillMap[coord.x][coord.y] == '.');
            fillMap[coord.x][coord.y] = 'O';
            numOutsideCells++;
            for (const auto& [dx, dy] : std::initializer_list<std::pair<int, int>>{ {-1, 0},
                    {+1, 0},
                    {0, -1},
                    {0, +1}
                    })
            {
                const int64_t newX = coord.x + dx;
                const int64_t newY = coord.y + dy;
                if (newX < 0 || newX >= width || newY < 0 || newY >= height)
                    continue;
                if (fillMap[newX][newY] == '#')
                    continue;
                if (seen.contains({newX, newY}))
                    continue;
                nextToExplore.push_back({newX, newY});
                seen.insert({newX, newY});
            }

        }

        toExplore = nextToExplore;
    }
    std::cout << "After fill: " << std::endl;
    for (int64_t y = 0; y < height; y++)
    {
        for (int64_t x = 0; x < width; x++)
        {
            cout << fillMap[x][y];
        }
        cout << endl;
    }
    std::cout << "numOutsideCells: " << numOutsideCells << std::endl;
    return width * height - numOutsideCells;
}
#endif

struct Line;
ostream& operator<<(ostream& os, const Line& line);
struct Line
{
    Coord begin;
    Coord end;
    Direction direction;
    Line *next = nullptr;

    int64_t length() const
    {
        return (abs(end.x - begin.x) + abs(end.y - begin.y)) + 1;
    }
    int dx() const
    {
        switch (direction)
        {
            case Right:
                return 1;
            case Left:
                return -1;
        }
        return 0;
    }
    int dy() const
    {
        switch (direction)
        {
            case Up:
                return -1;
            case Down:
                return +1;
        }
        return 0;
    }
    std::optional<int64_t> rightmostXOfIntersection(const int64_t horizLineY) const
    {
        if (direction == Up || direction == Down)
        {
            if (horizLineY < std::min(begin.y, end.y))
                return std::optional<int64_t>();
            if (horizLineY > std::max(begin.y, end.y))
                return std::optional<int64_t>();
            assert(begin.x == end.x);
            return begin.x;
        }
        else if (direction == Left || direction == Right)
        {
            assert(begin.y == end.y);
            if (horizLineY != begin.y)
            {
                //std::cout << "  " << *this << " does not intersect with horizLineY: " << horizLineY << std::endl;
                return std::optional<int64_t>();
            }
            return std::max(begin.x, end.x);
        }
        //std::cout << "whoops line: (" << begin.x << ", " << begin.y << ") - (" << end.x << ", " << end.y << ")" << std::endl;
        assert(false);
        return std::optional<int64_t>();
    }
    private:
    static int sgn(int i)
    {
        if (i < 0)
            return -1;
        else if (i == 0)
            return 0;
        else return +1;
    }
};
ostream& operator<<(ostream& os, const Line& line)
{
    os << "line: (" << line.begin.x << ", " << line.begin.y << ") - (" << line.end.x << ", " << line.end.y << ")";
    return os;
}


int64_t lagoonSizeOptimised(const vector<DigSection>& digPlan)
{
    int64_t x = 0;
    int64_t y = 0;
    vector<Line> digLines;
    for (const auto& digSection : digPlan)
    {
        int dx = 0, dy = 0;
        switch (digSection.direction)
        {
            case Up:
                dy = -1;
                break;
            case Right:
                dx = +1;
                break;
            case Down:
                dy = +1;
                break;
            case Left:
                dx = -1;
                break;
        }
        x += dx;
        y += dy;
        Line digLine;
        digLine.direction = digSection.direction;
        digLine.begin = { x, y };
        for (int64_t i = 0; i < digSection.numToDig - 1; i++)
        {
            x += dx;
            y += dy;
        }
        digLine.end = { x, y };
        digLines.push_back(digLine);
        std::cout << "line: (" << digLine.begin.x << ", " << digLine.begin.y << ") - (" << digLine.end.x << ", " << digLine.end.y << ")" << std::endl;
    }
    int64_t lowestHorzLineY = std::numeric_limits<int64_t>::max();
    Line lowestHorzLine;
    for (const auto& line : digLines)
    {
        if (line.direction != Left && line.direction != Right)
            continue;
        assert(line.begin.y == line.end.y);
        if (line.begin.y < lowestHorzLineY)
        {
            lowestHorzLineY = line.begin.y;
            lowestHorzLine = line;
        }
    }
    if (lowestHorzLine.direction == Right)
    {
        // Good; lines are in clockwise order.
        std::cout << "Lines are in clockwise order" << std::endl;
    }
    else
    {
        // Need to reverse line order.
        std::cout << "Lines were in *anti-*clockwise order; will reverse" << std::endl;
        std::reverse(digLines.begin(), digLines.end());
        for (auto& line : digLines)
        {
            std::swap(line.begin, line.end);
            switch (line.direction)
            {
                case Up:
                    line.direction = Down;
                    break;
                case Down:
                    line.direction = Up;
                    break;
                case Left:
                    line.direction = Right;
                    break;
                case Right:
                    line.direction = Left;
                    break;
            }
        }
    }
    Line* previousLine = &(digLines.back());
    for (auto& line : digLines | std::views::reverse)
    {
        // We're going through the lines backwards, so the "previous"
        // is "next".
        line.next = previousLine;
        previousLine = &line;
    }
    digLines.back().next = &(digLines.front());
    int64_t result = 0;
#ifdef BRUTE_FORCE
    std::map<Coord, int> numAtCoord;

    for (const auto& line : digLines)
    {
        int64_t x = line.begin.x;
        int64_t y = line.begin.y;
        const int dx = line.dx();
        const int dy = line.dy();
        for (int i = 0; i < line.length(); i++)
        {
            numAtCoord[{x, y}]++;
            x += dx;
            y += dy;
        }
    }

    auto printMap = [&numAtCoord]()
    {
        int64_t minX = std::numeric_limits<int64_t>::max();
        int64_t minY = std::numeric_limits<int64_t>::max();
        int64_t maxX = std::numeric_limits<int64_t>::min();
        int64_t maxY = std::numeric_limits<int64_t>::min();
        for (const auto& [coord, colour] : numAtCoord)
        {
            minX = std::min(minX, coord.x);
            minY = std::min(minY, coord.y);
            maxX = std::max(maxX, coord.x);
            maxY = std::max(maxY, coord.y);
        }
        const int64_t width = maxX - minX + 1;
        const int64_t height = maxY - minY + 1;
        assert(width > 0);
        assert(height > 0);
        vector<vector<char>> fillMap(width, vector<char>(height, '.'));
        for (const auto& [coord, num] : numAtCoord)
        {
            const int64_t x = coord.x - minX;
            const int64_t y = coord.y - minY;
            assert(x >= 0 && x < width);
            assert(y >= 0 && y < height);
            if (num == 1)
                fillMap[x][y] = '#';
            else
                fillMap[x][y] = num + '0';
        }
        for (int64_t y = 0; y < height; y++)
        {
            for (int64_t x = 0; x < width; x++)
            {
                cout << fillMap[x][y];
            }
            cout << endl;
        }
    };
    std::cout << "Optimised map (before fill): " << std::endl;
    printMap();
    vector<Line> horizFillLines;

#endif
    set<int64_t> yLineBreaks;
    for (const auto& line : digLines)
    {
        const auto lineTopY = std::min(line.begin.y, line.end.y);
        const auto lineBottomY = std::max(line.begin.y, line.end.y);
        yLineBreaks.insert(lineTopY - 1);
        yLineBreaks.insert(lineBottomY);
    }
    Direction previousDirection = None;
    for (const auto& line : digLines)
    {
        result += line.length();
        if (line.direction == Down)
        {
            int64_t pixelsFilledToLeft = 0;
            assert(line.begin.x == line.end.x);
            const int64_t lineX = line.begin.x;
            int64_t yLineSectionBegin = line.begin.y;
            auto nextYLineBreakIter = yLineBreaks.begin();
            while (yLineSectionBegin <= line.end.y)
            {
                while (*nextYLineBreakIter < yLineSectionBegin)
                {
                    nextYLineBreakIter++;
                    assert(nextYLineBreakIter != yLineBreaks.end());
                }
                optional<int64_t> nearestWallToleftX;
                for (const auto& otherLine : digLines)
                {
                    const auto wallToLeftX = otherLine.rightmostXOfIntersection(yLineSectionBegin);
                    //std::cout << "    otherLine: " << otherLine << " wallToLeftX: " << (wallToLeftX.has_value() ? std::to_string(wallToLeftX.value()) : "NONE") << std::endl;
                    assert(otherLine.rightmostXOfIntersection(*nextYLineBreakIter) == wallToLeftX);
                    if (!wallToLeftX.has_value() || wallToLeftX.value() >= lineX)
                    {
                        //std::cout << " ignoring otherLine: " << otherLine << std::endl;
                        continue;
                    }
                    if (!nearestWallToleftX.has_value() || (wallToLeftX.value() > nearestWallToleftX.value()))
                        nearestWallToleftX = wallToLeftX;
                }
                assert(nearestWallToleftX.has_value());
                assert(nearestWallToleftX.value() < lineX);
                if (nearestWallToleftX.value() + 1 <= lineX - 1)
                {
                    const int64_t horizontalFillLineLength = lineX - (nearestWallToleftX.value() + 1);
                    const int64_t pixelsFilledToLeftByLineSection = (*nextYLineBreakIter - yLineSectionBegin + 1) * horizontalFillLineLength;
                    pixelsFilledToLeft += pixelsFilledToLeftByLineSection;
                }
                yLineSectionBegin = *nextYLineBreakIter + 1;

                //std::cout << " lineX: " << lineX << " yInLine: " << yInLine << " nearestWallToleftX: " << (nearestWallToleftX.has_value() ? std::to_string(nearestWallToleftX.value()) : "NONE") << std::endl;
            }
#ifdef BRUTE_FORCE
            int64_t dbgTotalPixelsFilled = 0;
            {
                for (int yInLine = line.begin.y; yInLine <= line.end.y; yInLine++)
                {
                    std::cout << "Trying to find  nearestWallToleftX for line: " << line << " yInLine: " << yInLine << std::endl;
                    optional<int64_t> nearestWallToleftX;
                    for (const auto& otherLine : digLines)
                    {
                        const auto wallToLeftX = otherLine.rightmostXOfIntersection(yInLine);
                        //std::cout << "    otherLine: " << otherLine << " wallToLeftX: " << (wallToLeftX.has_value() ? std::to_string(wallToLeftX.value()) : "NONE") << std::endl;
                        if (!wallToLeftX.has_value() || wallToLeftX.value() >= lineX)
                        {
                            //std::cout << " ignoring otherLine: " << otherLine << std::endl;
                            continue;
                        }
                        if (!nearestWallToleftX.has_value() || (wallToLeftX.value() > nearestWallToleftX.value()))
                            nearestWallToleftX = wallToLeftX;
                    }
                    std::cout << " lineX: " << lineX << " yInLine: " << yInLine << " nearestWallToleftX: " << (nearestWallToleftX.has_value() ? std::to_string(nearestWallToleftX.value()) : "NONE") << std::endl;
                    assert(nearestWallToleftX.value() < lineX);
                    if (nearestWallToleftX.value() + 1 <= lineX - 1)
                    {
                        for (int64_t fillX = nearestWallToleftX.value() + 1; fillX < lineX; fillX++)
                        {
                            //std::cout << " filling: " << fillX << ", " << yInLine << std::endl;
                            assert(!numAtCoord.contains({fillX, yInLine}));
                            numAtCoord[{fillX, yInLine}]++;
                            dbgTotalPixelsFilled++;
                        }
                        horizFillLines.push_back({{nearestWallToleftX.value() + 1, yInLine}, {lineX - 1, yInLine}});
                        //std::cout << "Added horizFillLine: " << horizFillLines.back() << " length: " << horizFillLines.back().length() << " blee: " << blee << std::endl;
                    }
                    //assert(horizFillLines.back().length() == blee);
                }
            }
            assert(dbgTotalPixelsFilled == pixelsFilledToLeft);
#endif
            result += pixelsFilledToLeft;
        }
        else  if (line.direction == Left/* && previousDirection == Down)*/ && line.next->direction == Down)
        {
            int64_t blee = 0;
            std::cout << "Trying to find  nearestWallToleftX for horizontal line: " << line << std::endl;
            optional<int64_t> nearestWallToleftX;
            assert(line.end.x <= line.begin.x);
            const int64_t lineLeftX = line.end.x;
            assert(line.begin.y == line.end.y);
            const int64_t lineY = line.begin.y;
            for (const auto& otherLine : digLines)
            {
                const auto wallToLeftX = otherLine.rightmostXOfIntersection(lineY);
                //std::cout << "    otherLine: " << otherLine << " wallToLeftX: " << (wallToLeftX.has_value() ? std::to_string(wallToLeftX.value()) : "NONE") << std::endl;
                if (!wallToLeftX.has_value() || wallToLeftX.value() >= lineLeftX)
                {
                    //std::cout << " ignoring otherLine: " << otherLine << std::endl;
                    continue;
                }
                if (!nearestWallToleftX.has_value() || (wallToLeftX.value() > nearestWallToleftX.value()))
                {
                    //std::cout << "  new best line: " << otherLine << " wallToLeftX: " << wallToLeftX.value();
                    nearestWallToleftX = wallToLeftX;
                }

            }
            if (!nearestWallToleftX.has_value())
            {
                std::cout << "   none found" << std::endl;
                continue;
            }
            assert(nearestWallToleftX.has_value());
            assert(nearestWallToleftX.value() < lineLeftX);
            const int64_t horizontalFillLineLength = lineLeftX - (nearestWallToleftX.value() + 1);
            result += horizontalFillLineLength;
#ifdef BRUTE_FORCE
            for (int64_t fillX = nearestWallToleftX.value() + 1; fillX < lineLeftX; fillX++)
            {
                //std::cout << " filling: " << fillX << ", " << yInLine << std::endl;
                assert(!numAtCoord.contains({fillX, lineY}));
                numAtCoord[{fillX, lineY}]++;
                blee++;
            }
            horizFillLines.push_back({{nearestWallToleftX.value() + 1, lineY}, {lineLeftX - 1, lineY}});
            assert(horizFillLines.back().length() == blee);
#endif

        }
        previousDirection = line.direction;
    }

#ifdef BRUTE_FORCE
    {
        int64_t minX = std::numeric_limits<int64_t>::max();
        int64_t minY = std::numeric_limits<int64_t>::max();
        int64_t maxX = std::numeric_limits<int64_t>::min();
        int64_t maxY = std::numeric_limits<int64_t>::min();
        for (const auto& [coord, colour] : numAtCoord)
        {
            minX = std::min(minX, coord.x);
            minY = std::min(minY, coord.y);
            maxX = std::max(maxX, coord.x);
            maxY = std::max(maxY, coord.y);
        }
        const int64_t width = maxX - minX + 1;
        const int64_t height = maxY - minY + 1;
        assert(width > 0);
        assert(height > 0);
        vector<vector<char>> fillMap(width, vector<char>(height, '.'));
        for (const auto& line : digLines)
        {
            int64_t x = line.begin.x;
            int64_t y = line.begin.y;
            const int dx = line.dx();
            const int dy = line.dy();
            for (int i = 0; i < line.length(); i++)
            {
                fillMap[x - minX][y - minY] = '#';
                x += dx;
                y += dy;
            }
        }
        for (const auto& line : horizFillLines)
        {
            for (int x = line.begin.x; x <= line.end.x; x++)
            {
                if (fillMap[x - minX][line.begin.y - minY] == '.')
                    fillMap[x - minX][line.begin.y - minY] = 'O';
                else
                    fillMap[x - minX][line.begin.y - minY] = 'X';
            }
        }
        std::cout << "optimised width: " << width << " height: " << height << std::endl;
        ofstream out("gloop.rgb", std::ios::binary);
        unsigned char rgbBlack[] = { 0, 0, 0 };
        unsigned char rgbWhite[] = { 255, 255, 255 };
        unsigned char rgbRed[] = { 255, 0, 0 };
        unsigned char rgbGreen[] = { 0, 255, 0 };
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                if (fillMap[x][y] == '#')
                {
                    out.write(reinterpret_cast<char*>(rgbBlack), 3);
                }
                else if (fillMap[x][y] == 'O')
                {
                    out.write(reinterpret_cast<char*>(rgbGreen), 3);
                }
                else if (fillMap[x][y] == 'X')
                {
                    assert(false);
                    out.write(reinterpret_cast<char*>(rgbWhite), 3);
                }
                else
                    out.write(reinterpret_cast<char*>(rgbRed), 3);

            }
        }
        out.flush();
        assert(out);
        out.close();
    }
    std::cout << "Optimised map after fill:" << std::endl;
    printMap();
#endif
#if 0
    int64_t result = 0;
    for (const auto& line : digLines)
        result += line.length();
    for (const auto& line : horizFillLines)
        result += line.length();
#endif
    return result;
}


int main()
{
    std::regex digPlanRegex(R"(^([URDL])\s*(\d+)\s*\((.*)\)\s*$)");
    string digPlanLine;

    vector<DigSection> digPlan;
    while (getline(cin, digPlanLine))
    {
        std::smatch digPlanMatch;

        const bool matchSuccessful = std::regex_match(digPlanLine, digPlanMatch, digPlanRegex);
        assert(matchSuccessful);

        string colourRGB = digPlanMatch[3];
        const auto dirChar = colourRGB.back();

        // Remove dirChar and the leading "#" from colourRGB.
        colourRGB.erase(colourRGB.begin());
        colourRGB.pop_back();
        std::cout << "numToDig as hex: " << colourRGB << std::endl;
        int64_t numToDig = 0;
        for (const auto& hexDigit : colourRGB)
        {
            int digitalDecimal = 0;
            if (hexDigit >= '0' && hexDigit <= '9')
                digitalDecimal = hexDigit - '0';
            else if (hexDigit >= 'a' && hexDigit <= 'f')
                digitalDecimal = hexDigit - 'a' + 10;
            else
                assert(false);
            numToDig *= 16;
            numToDig += digitalDecimal;
        }

        std::cout << "dirChar: " << dirChar << " numToDig: " << numToDig << " colourRGB: " << colourRGB << std::endl;

        DigSection digSection;
        digSection.numToDig = numToDig;
        switch (dirChar)
        {
            case '0':
                digSection.direction = Right;
                break;
            case '1':
                digSection.direction = Down;
                break;
            case '2':
                digSection.direction = Left;
                break;
            case '3':
                digSection.direction = Up;
                break;
        }
        digPlan.push_back(digSection);
    }

    const int64_t resultOptimised = lagoonSizeOptimised(digPlan);
    std::cout << "result: " << resultOptimised << std::endl;


}
