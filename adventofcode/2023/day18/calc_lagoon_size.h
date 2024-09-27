#ifndef CALC_LAGOON_SIZE_H
#define CALC_LAGOON_SIZE_H

#include <iostream>
#include <vector>
#include <set>
#include <optional>
#include <ranges>
#include <algorithm>
#include <limits>

#include <cassert>

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
    std::string colourRGB;
};

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
                return std::optional<int64_t>();
            return std::max(begin.x, end.x);
        }
        return std::optional<int64_t>();
    }
};

int64_t lagoonSizeOptimised(const std::vector<DigSection>& digPlan)
{
    int64_t x = 0;
    int64_t y = 0;
    std::vector<Line> digLines;
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

        x += dx * (digSection.numToDig - 1);
        y += dy * (digSection.numToDig - 1);
        digLine.end = { x, y };
        digLines.push_back(digLine);
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
    std::set<int64_t> yLineBreaks;
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
                std::optional<int64_t> nearestWallToleftX;
                for (const auto& otherLine : digLines)
                {
                    const auto wallToLeftX = otherLine.rightmostXOfIntersection(yLineSectionBegin);
                    assert(otherLine.rightmostXOfIntersection(*nextYLineBreakIter) == wallToLeftX);
                    if (!wallToLeftX.has_value() || wallToLeftX.value() >= lineX)
                    {
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
            }
            result += pixelsFilledToLeft;
        }
        else  if ((line.direction == Left) && (line.next->direction == Down))
        {
            std::optional<int64_t> nearestWallToleftX;
            assert(line.end.x <= line.begin.x);
            const int64_t lineLeftX = line.end.x;
            assert(line.begin.y == line.end.y);
            const int64_t lineY = line.begin.y;
            for (const auto& otherLine : digLines)
            {
                const auto wallToLeftX = otherLine.rightmostXOfIntersection(lineY);
                if (!wallToLeftX.has_value() || wallToLeftX.value() >= lineLeftX)
                {
                    continue;
                }
                if (!nearestWallToleftX.has_value() || (wallToLeftX.value() > nearestWallToleftX.value()))
                {
                    nearestWallToleftX = wallToLeftX;
                }

            }
            assert(nearestWallToleftX.has_value());
            assert(nearestWallToleftX.has_value());
            assert(nearestWallToleftX.value() < lineLeftX);
            const int64_t horizontalFillLineLength = lineLeftX - (nearestWallToleftX.value() + 1);
            result += horizontalFillLineLength;

        }
        previousDirection = line.direction;
    }

    return result;
}

#endif

