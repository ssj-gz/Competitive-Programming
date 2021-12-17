#include <iostream>
#include <regex>
#include <cassert>

using namespace std;

int main()
{
    string targetAreaLine;
    getline(cin, targetAreaLine);

    static regex targetAreaRegex(R"(target area: x=([-\d]+)\.\.([-\d]+),\s*y=([-\d]+)\.\.([-\d]+)\s*)");
    std::smatch targetAreaMatch;
    int targetAreaLeft = -1;
    int targetAreaRight = -1;
    int targetAreaTop = -1;
    int targetAreaBottom = -1;
    if (regex_match(targetAreaLine, targetAreaMatch, targetAreaRegex))
    {
        targetAreaLeft = stoi(targetAreaMatch[1]);
        targetAreaRight = stoi(targetAreaMatch[2]);
        assert(targetAreaLeft <= targetAreaRight);
        targetAreaTop = stoi(targetAreaMatch[4]);
        targetAreaBottom = stoi(targetAreaMatch[3]);

        assert(targetAreaBottom <= targetAreaTop);
    }
    else
    {
        assert(false);
    }

    cout << "targetAreaLeft: " << targetAreaLeft << endl;
    cout << "targetAreaRight: " << targetAreaRight << endl;
    cout << "targetAreaTop: " << targetAreaTop << endl;
    cout << "targetAreaBottom: " << targetAreaBottom << endl;

    assert((targetAreaLeft > 0 && targetAreaRight > 0) || (targetAreaLeft < 0 && targetAreaRight < 0));
    assert((targetAreaTop > 0 && targetAreaBottom > 0) || (targetAreaTop < 0 && targetAreaBottom < 0));
    int xDir = (targetAreaLeft > 0) ? 1 : -1;

    int bestEverY = 0;
    int numSuccessful = 0;
    for (int candidateXVel = 0; abs(candidateXVel) <= max(abs(targetAreaLeft), abs(targetAreaRight)); candidateXVel += xDir)
    {
        bool stopIncreasingY = false;
        for (int candidateYVel = -max(abs(targetAreaTop), abs(targetAreaBottom)); /*!stopIncreasingY*/ abs(candidateYVel) < 1'000; candidateYVel++)
        {
            int xVel = candidateXVel;
            int yVel = candidateYVel;
            cout << "Simulating xVel = " << xVel << " yVel = " << yVel << endl;
            // Simulate.
            int x = 0;
            int y = 0;
            int maxY = 0;
            bool successful = false;
            do
            {
                cout << " x: " << x << " y: " << y << endl;
                x += xVel;
                y += yVel;
                maxY = max(maxY, y);
                if (targetAreaLeft <= x && x <= targetAreaRight &&
                   targetAreaTop >= y && y >= targetAreaBottom)
                {
                    successful = true;
                    break;
                }
                if (xVel > 0)
                {
                    if (x > targetAreaRight)
                    {
                        if (y > targetAreaTop)
                        {
                            stopIncreasingY = true;
                        }
                        break;
                    }
                }
                else if (xVel < 0)
                {
                    if (x < targetAreaLeft)
                    {
                        if (y > targetAreaTop)
                            stopIncreasingY = true;
                        break;
                    }
                }
                else 
                {
                    if (x < targetAreaLeft || x > targetAreaRight || y < targetAreaBottom)
                    {
                        if (y > targetAreaTop)
                            stopIncreasingY = true;
                        break;
                    }
                }
                if (yVel < 0 && y < targetAreaBottom)
                    break;
                if (xVel > 0)
                    xVel--;
                else if (xVel < 0)
                    xVel++;
                yVel--;
            }
            while(true);
            cout << "Successful?" << successful << " ended at x: " << x << " y: " << y << " candidateXVel: " << candidateXVel << " candidateYVel: " << candidateYVel << " maxY: " << maxY << endl;
            if (successful)
            {
                bestEverY = max(bestEverY, maxY);
                numSuccessful++;
            }
        }
    }
    cout << "bestEverY:" << bestEverY << endl;
    cout << "numSuccessful: " << numSuccessful << endl;


}
