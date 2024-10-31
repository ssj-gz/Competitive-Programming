#include <iostream>
#include <vector>

#include <cassert>

using namespace std;

int main()
{
    vector<string> rawDiagram;
    string rawDiagramLine;
    while (getline(cin, rawDiagramLine))
    {
        rawDiagram.push_back(rawDiagramLine);
    }
    const int width = rawDiagram.front().size();
    const int height = rawDiagram.size();

    vector<vector<char>> diagram(width, vector<char>(height, ' '));
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            diagram[x][y] = rawDiagram[y][x];
        }
    }

    int startX = -1;
    const int startY = 0;
    for (int x = 0; x < width; x++)
    {
        if (diagram[x][0] == '|')
        {
            assert(startX == -1);
            startX = x;
        }
    }
    assert(startX != -1);

    enum Direction { Up, Right, Down, Left };
    auto dxdy = [](const Direction direction)
    {
        int dx = 0;
        int dy = 0;
        switch (direction)
        {
            case Up:
                dy = -1;
                break;
            case Right:
                dx = 1;
                break;
            case Down:
                dy = 1;
                break;
            case Left:
                dx = -1;
                break;
        };
        return std::pair {dx, dy};
    };
    auto isAllowedTrackType = [](Direction direction, char trackType)
    {
        std::cout << " isAllowedTrackType - direction: " << direction << " trackType: " << trackType << std::endl;
        if (trackType == '+' || (trackType >= 'A' && trackType <= 'Z'))
            return true;
        return (trackType == '|') || (trackType == '-');
#if 0
        switch (direction)
        {
            case Up:
            case Down:
                std::cout << " returning: " << (trackType == '|') << std::endl;
                return trackType == '|';
            case Left:
            case Right:
                std::cout << " returning: " << (trackType == '-') << std::endl;
                return trackType == '-';
        }
#endif
        //assert(false);
        return false;
    };

    int x = startX;
    int y = startY;

    Direction direction = Down;
    auto trackTypeInDirection = [&x, &y, &diagram, &direction, &dxdy, width, height]()
    {
        auto [dx, dy] = dxdy(direction);
        const int newX = x + dx;
        const int newY = y + dy;
        if (newX < 0 || newX >= width)
            return '#';
        if (newY < 0 || newY >= height)
            return '#';
        return diagram[newX][newY];
    };
    int numSteps = 1; // Includes entering the diagram.
    while (true)
    {
        std::cout << "x: "<< x << " y: " << y << " direction: " << direction << " trackTypeInDirection: " << trackTypeInDirection() << std::endl;
        assert(x >= 0 && x < width);
        assert(y >= 0 && y < height);
        if (!isAllowedTrackType(direction, trackTypeInDirection()))
        {
            std::cout << "Uh-oh - need to turn!" << std::endl;
            if (diagram[x][y] == '+')
            {
                std::cout << "can turn here" << std::endl;
                const auto origDirection = direction;
                // Turn right.
                direction = static_cast<Direction>((static_cast<int>(direction) + 1) % 4);
                if (!isAllowedTrackType(direction, trackTypeInDirection()))
                {
                    // Nope.  Left?
                    direction = static_cast<Direction>((static_cast<int>(origDirection) + 3) % 4);
                    if (!isAllowedTrackType(direction, trackTypeInDirection()))
                    {
                        // Nope - end of the line!
                        break;
                    }
                    else
                    {
                        std::cout << "Turned left successfully" << std::endl;
                    }
                }
                else
                {
                    std::cout << "Turned right successfully" << std::endl;
                }
                
            }
            else
            {
                // End of the line.
                break;
            }
        }
        else
        {
            auto [dx, dy] = dxdy(direction);
            x += dx;
            y += dy;
            numSteps++;
        }
    }
    std::cout << "end of the line!" << std::endl;
    std::cout << "numSteps: " << numSteps << std::endl;


    
    return 0;
}
