#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

#define _GLIBCXX_DEBUG       // Iterator safety; out-of-bounds access for Containers, etc.
#pragma GCC optimize "trapv" // abort() on (signed) integer overflow.

using namespace std;

int main()
{
    string jetBlastPatterns;
    cin >> jetBlastPatterns;

    std::cout << "jetBlastPatterns: " << jetBlastPatterns << std::endl;

    const vector<vector<string>> shapes = {
        { "####" },

        { ".#.",
          "###",
          ".#." },

        { "..#",
          "..#",
          "###" },

        { "#",
          "#",
          "#",
          "#" },

        { "##",
          "##" }
    };

    constexpr int playAreaWidth = 7;
    constexpr int playAreaMinX = 0;
    constexpr int playAreaMaxX = playAreaMinX + playAreaWidth - 1;
    // The "back" of the playArea is the "topmost" row;
    // the front, the bottommost.
    vector<string> playArea = { string(playAreaWidth, '-') };
    int jetStreamIndex = 0;
    int nextShapeIndex = 0;


    int numRocksLanded = 0;
    while (true)
    {
        // Next shape appears.
        const auto& shape = shapes[nextShapeIndex];
        const int shapeHeight = shape.size();
        const int shapeWidth = shape.front().size();
        int shapeLeftX = 2;
        int shapeTopY = playArea.size() + 3 + shape.size() - 1;
        std::cout << "new shape appears" << std::endl;
        std::cout << "shapeLeftX: " << shapeLeftX << " shapeTopY: " << shapeTopY << " shapeWidth: " << shapeWidth << " shapeHeight: " << shapeHeight << std::endl;

        auto drawPlayArea = [&]()
        {
            return;
            //std::cout << "drawPlayArea: shapeLeftX: " << shapeLeftX << " shapeTopY: " << shapeTopY << std::endl;
            vector<string> playAreaView(max<int>(shapeTopY + 1, playArea.size() + 1), string(playAreaWidth, '.'));
            for (int y = 0; y < playArea.size(); y++)
            {
                for (int x = 0; x < playAreaWidth; x++)
                {
                    playAreaView[y][x] = playArea[y][x];
                }
            }
            for (int x = 0; x < shapeWidth; x++)
            {
                for (int y = 0; y < shapeHeight; y++)
                {
                    //std::cout << "y: " << y << std::endl;
                    const int xInPlayAreaView = x + shapeLeftX;
                    const int yInPlayAreaView = shapeTopY - y;
                    //std::cout << "yInPlayAreaView: " << y + yInPlayAreaView << std::endl;

                    assert(0 <= yInPlayAreaView);
                    assert(yInPlayAreaView < playAreaView.size());
                    assert(0 <= xInPlayAreaView);
                    assert(xInPlayAreaView < playAreaView.front().size());

                    if (shape[y][x] == '#')
                        playAreaView[yInPlayAreaView][xInPlayAreaView] = '@';
                    else
                        playAreaView[yInPlayAreaView][xInPlayAreaView] = '.';

                }
            }
            std::reverse(playAreaView.begin(), playAreaView.end());
            std::cout << "Current: " << std::endl;
            for (const auto& row : playAreaView)
            {
                std::cout << row << std::endl;
            }
        };

        std::cout << "Shape appears" << std::endl;
        drawPlayArea();
        while (true)
        {
            const int origShapeLeftX = shapeLeftX;

            std::cout << "Jet attempts to blow to " << jetBlastPatterns[jetStreamIndex] << std::endl;
            if (jetBlastPatterns[jetStreamIndex] == '<')
            {
                shapeLeftX--;
            }
            else
            {
                shapeLeftX++;
            }
            jetStreamIndex = (jetStreamIndex + 1) % jetBlastPatterns.size();

            auto isShapePosValid = [&]()
            {
                bool isValid = true;
                for (int x = 0; x < shapeWidth; x++)
                {
                    for (int y = 0; y < shapeHeight; y++)
                    {
                        //std::cout << "y: " << y << std::endl;
                        const int xInPlayAreaView = x + shapeLeftX;
                        const int yInPlayAreaView = shapeTopY - y;
                        if (xInPlayAreaView < 0 || xInPlayAreaView >= playAreaWidth)
                        {
                            return false;
                        }
                        if (yInPlayAreaView < 0)
                        {
                            return false;
                        }
                        if (yInPlayAreaView >= playArea.size())
                            continue; // Fine - shape is too high up to touch anything.
                        if (shape[y][x] != '.' && playArea[yInPlayAreaView][xInPlayAreaView] != '.')
                        {
                            return false;
                        }
                    }
                }
                return true;
            };
            if (!isShapePosValid())
            {
                std::cout << " stream didn't push us" << std::endl;
                shapeLeftX = origShapeLeftX;
            }
            drawPlayArea();
            std::cout << "Attempting to descend" << std::endl;
            const int origShapeTopY = shapeTopY;
            shapeTopY--;
            if (!isShapePosValid())
            {
                std::cout << " couldn't settle" << std::endl;
                shapeTopY = origShapeTopY;
                // Inscribe in play area.
                std::cout << "shapeTopY: " << shapeTopY << std::endl;
                for (int x = 0; x < shapeWidth; x++)
                {
                    for (int y = 0; y < shapeHeight; y++)
                    {
                        //std::cout << "y: " << y << std::endl;
                        const int xInPlayArea = x + shapeLeftX;
                        const int yInPlayArea = shapeTopY - y;
                        while (yInPlayArea >= playArea.size())
                            playArea.push_back(string(playAreaWidth, '.'));
                        playArea[yInPlayArea][xInPlayArea] = shape[y][x];
                    }
                }
                std::cout << "Inscribed shape" << std::endl;
                drawPlayArea();
                numRocksLanded++;

                break;
            }
            std::cout << "Descent succeeded" << std::endl;
            drawPlayArea();
        }
        std::cout << "numRocksLanded: " << numRocksLanded << std::endl;
        if (numRocksLanded == 2022)
        {
            std::cout << "answer: " << playArea.size() - 1 /* Ignore "------" */ << std::endl;
            break;
        }


        nextShapeIndex = (nextShapeIndex + 1) % shapes.size();
    }

}
