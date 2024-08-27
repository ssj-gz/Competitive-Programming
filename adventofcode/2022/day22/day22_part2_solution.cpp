#include <iostream>
#include <vector>
#include <deque>
#include <map>
#include <sstream>
#include <ranges>
#include <algorithm>
#include <limits>
#include <cassert>

using namespace std;

struct Coord3d
{
    public:
        Coord3d() = default;
        Coord3d(int x, int y, int z)
            : m_x{x},
              m_y{y},
              m_z{z}
        {
        }
        auto operator<=>(const Coord3d& other) const = default;
        int x() const { return m_x; };
        int y() const { return m_y; };
        int z() const { return m_z; };
    private:
        int m_x = -1;
        int m_y = -1;
        int m_z = -1;
};

Coord3d operator+(const Coord3d& lhs, const Coord3d& rhs)
{
    return {lhs.x() + rhs.x(), lhs.y() + rhs.y(), lhs.z() + rhs.z()};
}

Coord3d operator*(int factor, const Coord3d& coord)
{
    return { coord.x() * factor, coord.y() * factor, coord.z() * factor };
}

Coord3d operator*(const Coord3d& coord, int factor)
{
    return factor * coord;
}

Coord3d operator-(const Coord3d& lhs, const Coord3d& rhs)
{
    return lhs + (-1 * rhs);
}

struct Face
{
    public:
       Face(const vector<vector<char>>& cubeMap, int tlX, int tlY, int size)
           : m_tlX{tlX},
             m_tlY{tlY},
             m_size{size}
       {
           m_topLeft = {tlX, tlY, 0};
           m_downFromTopLeft = {tlX, tlY + 1, 0};
           m_rightOfTopLeft = {tlX + 1, tlY, 0};
           m_outFromTopLeft = {tlX, tlY, 1};
           auto cellCoords = this->cellCoords();
           for (int x = tlX; x < tlX + size; x++)
           {
               for (int y = tlY; y < tlY + size; y++)
               {
                   assert(cubeMap[x][y] != ' ');
                   m_cellContents[cellCoords[x - tlX][y - tlY]] = cubeMap[x][y];
               }
           }
       }
       void addNeighbour(Face* neighbour)
       {
           m_neighbours.push_back(neighbour);
       }
       vector<Face*> neighbours() const
       {
           return m_neighbours;
       }
       int tlX() const
       {
           return m_tlX;
       }
       int tlY() const
       {
           return m_tlY;
       }

       Coord3d topLeft()
       {
           return m_topLeft;
       }
       Coord3d rightOfTopLeft()
       {
           return m_rightOfTopLeft;
       }
       Coord3d downFromTopLeft()
       {
           return m_downFromTopLeft;
       }
       Coord3d outFromTopLeft()
       {
           return m_outFromTopLeft;
       }

       vector<vector<Coord3d>> cellCoords() const
       {
           vector<vector<Coord3d>> cellCoords(m_size, vector<Coord3d>(m_size));
           for (int x = 0; x < m_size; x++)
           {
               for (int y = 0; y < m_size; y++)
               {
                   cellCoords[x][y] = m_topLeft + x * (m_rightOfTopLeft - m_topLeft) + y * (m_downFromTopLeft - m_topLeft);
               }
           }
           return cellCoords;
       }

       char cellContents(const Coord3d& faceCell) const
       {
           auto cellContentIter = m_cellContents.find(faceCell);
           assert (cellContentIter != m_cellContents.end());
           return cellContentIter->second;
       }

    private:
       int m_tlX = -1;
       int m_tlY = -1;
       int m_size = -1;

       Coord3d m_topLeft;
       Coord3d m_rightOfTopLeft;
       Coord3d m_downFromTopLeft;
       Coord3d m_outFromTopLeft;

       vector<Face*> m_neighbours;

       std::map<Coord3d, char> m_cellContents;

};

void printFaces(const deque<Face>& faces, const vector<vector<char>>& cubeMap, const int faceSize)
{
    std::map<Coord3d, char> charAtCoord;
    int maxX = std::numeric_limits<int>::min();
    int maxY = std::numeric_limits<int>::min();
    int maxZ = std::numeric_limits<int>::min();
    int minX = std::numeric_limits<int>::max();
    int minY = std::numeric_limits<int>::max();
    int minZ = std::numeric_limits<int>::max();
    for (const auto& face : faces)
    {
        const auto& faceCellCoords = face.cellCoords();
        for (int x = face.tlX(); x < face.tlX() + faceSize; x++)
        {
            for (int y = face.tlY(); y < face.tlY() + faceSize; y++)
            {
                const auto faceCellCoord = faceCellCoords[x - face.tlX()][y - face.tlY()];
                charAtCoord[faceCellCoord] = face.cellContents(faceCellCoord);
                minX = std::min(minX, faceCellCoord.x());
                maxX = std::max(maxX, faceCellCoord.x());
                minY = std::min(minY, faceCellCoord.y());
                maxY = std::max(maxY, faceCellCoord.y());
                minZ = std::min(minZ, faceCellCoord.z());
                maxZ = std::max(maxZ, faceCellCoord.z());
            }
        }
    }
    std::cout << "maxZ: " << maxZ << " minZ: " << minZ << std::endl;
    for (int z = maxZ; z >= minZ; z--)
    {
        std::cout << "z: " << z << std::endl;
        //for (int y = maxY; y >= minY; y--)
        for (int y = minY; y <= maxY; y++)
        {
            for (int x = minX; x <= maxX; x++)
            {
                const auto coord = Coord3d{x, y, z};
                if (charAtCoord.contains(coord))
                {
                    cout << charAtCoord[coord];
                }
                else
                {
                    cout << ' ';
                }
            }
            cout << std::endl;
        }
    }
}

int main()
{
    vector<string> rawMapData;
    string rawMapLine;
    while (std::getline(cin, rawMapLine) && !rawMapLine.empty())
    {
        rawMapData.push_back(rawMapLine);
    }

#if 0
    for (const auto& rawMapLine : rawMapData)
    {
        std::cout << rawMapLine << std::endl;
    }
#endif


    const int width = std::ranges::max(rawMapData | std::ranges::views::transform([](const auto& line) { return line.size(); }));
    std::cout << "width: " << width << std::endl;
    const int height = rawMapData.size();
    const int startX = rawMapData.front().find('.');

    vector<vector<char>> map(width, vector<char>(height, ' '));
    int numNonSpaces = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < std::min<int>(width, rawMapData[y].size()); x++)
        {
            {
                map[x][y] = rawMapData[y][x];
                if (map[x][y] != ' ')
                    numNonSpaces++;
            }
        }
    }
    assert(numNonSpaces == 50 * 50 * 6 || numNonSpaces == 4 * 4 * 6);
    const bool isSample = (numNonSpaces == 4 * 4 * 6);
    const int faceSize = isSample ? 4 : 50;
    vector<vector<char>> pathDisplayMap = map;

    string path;
    cin >> path;

    deque<Face> faces;
    for (int faceTLX = 0; faceTLX < width; faceTLX += faceSize)
    {
        for (int faceTLY = 0; faceTLY < height; faceTLY += faceSize)
        {
            if (map[faceTLX][faceTLY] != ' ')
            {
                faces.push_back(Face(map, faceTLX, faceTLY, faceSize));
            }
        }
    }
    assert(faces.size() == 6);

    for (auto& face1 : faces)
    {
        for (auto& face2 : faces)
        {
            if (abs(face1.tlX() - face2.tlX()) + abs(face1.tlY() - face2.tlY()) == faceSize)
            {
                face1.addNeighbour(&face2);
            }
        }
    }
    for (auto& face : faces)
    {
        std::cout << "Face " << &face << " at: " << face.tlX() << ", " << face.tlY() << " has neighbours: " << std::endl;
        for (const auto* neighbour : face.neighbours())
            std::cout << " " << neighbour << " at: " << neighbour->tlX() << ", " << neighbour->tlY() << std::endl;
    }

    printFaces(faces, map, faceSize);

    return 0; // TODO - remove this


    std::cout << "path: " << path << std::endl;
    std::cout << "startX: " << startX << std::endl;

    istringstream pathStream(path);
    int x = startX;
    int y = 0;
    int direction = 0; // Facing right.

    auto markPositionOnDisplayMap = [&pathDisplayMap, &x, &y, &direction]()
    {
        char directionDisplayChar = '\0';
        switch (direction)
        {
            case 0:
                directionDisplayChar = '>';
                break;
            case 1:
                directionDisplayChar = 'V';
                break;
            case 2:
                directionDisplayChar = '<';
                break;
            case 3:
                directionDisplayChar = '^';
                break;
        };
        assert(directionDisplayChar != '\0');
        pathDisplayMap[x][y] = directionDisplayChar;
    };

    auto printDisplayMap = [&pathDisplayMap, width, height]()
    {
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                std::cout << pathDisplayMap[x][y];
            }
            std::cout << std::endl;
        }
    };

    markPositionOnDisplayMap();
    printDisplayMap();
    while (true)
    {
        int numToMove = 0;
        pathStream >> numToMove;
        if (!pathStream)
            break;
        std::cout << "Forward " << numToMove << std::endl;
        for (int step = 0; step < numToMove; step++)
        {
            const int origX = x;
            const int origY = y;
            while (true)
            {
                switch (direction)
                {
                    case 0:
                        x++;
                        break;
                    case 1:
                        y++;
                        break;
                    case 2:
                        x--;
                        break;
                    case 3:
                        y--;
                        break;
                };
                if (x < 0)
                    x = width -1;
                if (x >= width)
                    x = 0;
                if (y < 0)
                    y = height - 1;
                if (y >= height)
                    y = 0;
                if (map[x][y] != ' ')
                    break;
            }
            assert(x >= 0 && x < width && y >= 0 && y < height);
            if (map[x][y] == '#')
            {
                x = origX;
                y = origY;
            }
            else
            {
                printDisplayMap();
                std::cout << "Ended up at: " << x << ", " << y << " blah: " << map[x][y] << " width: " << width << " height: " << height << std::endl;
                assert(map[x][y] == '.');
                markPositionOnDisplayMap();
            }
        }
        std::cout << "Now at: " << x << ", " << y << " facing: " << direction << std::endl;
        char turnDir = '\0';
        pathStream >> turnDir; 
        if (!turnDir)
            break;
        assert((turnDir == 'L') || (turnDir == 'R'));
        std::cout << "Turn: " << turnDir << std::endl;
        if (turnDir == 'R')
            direction = (direction + 1) % 4;
        else
            direction = (4 + direction - 1) % 4;
        std::cout << "Now at: " << x << ", " << y << " facing: " << direction << std::endl;
        markPositionOnDisplayMap();
    }

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            std::cout << pathDisplayMap[x][y];
        }
        std::cout << std::endl;
    }

    std::cout << "password: " << ((y + 1) * 1000) + ((x + 1) * 4) + direction << std::endl;


}

