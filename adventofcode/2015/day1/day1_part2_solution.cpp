#include <iostream>
#include <algorithm>

#include <cassert>

using namespace std;

int main()
{
    string directions;
    std::getline(cin, directions);
    int floor = 0;
    for (string::size_type pos = 0; pos < directions.size(); pos++)
    {
        if (directions[pos] == '(')
            floor++;
        else
            floor--;
        if (floor == -1)
        {
            std::cout << "First pos leading to floor -1: " << (pos + 1) << std::endl;
            break;
        }
    }

    return 0;
}
