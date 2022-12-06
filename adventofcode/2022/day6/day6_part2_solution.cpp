#include <iostream>
#include <algorithm>
#include <iterator>
#include <cassert>

using namespace std;

int main()
{
    char lastFourChars[14] = {};
    string dataStream;
    cin >> dataStream;

    for (int pos = 0; pos < static_cast<int>(dataStream.size()); pos++)
    {
        std::rotate(std::begin(lastFourChars), std::next(std::begin(lastFourChars)), std::end(lastFourChars));
        lastFourChars[std::size(lastFourChars) - 1] = dataStream[pos];

        std::cout << "pos: " << pos << " lastFourChars: " << std::string(lastFourChars, std::size(lastFourChars) ) << std::endl;

        if (pos >= std::size(lastFourChars) - 1)
        {
            bool areAllDistinct = true;
            for (const auto letter : lastFourChars)
            {
                if (std::count(std::begin(lastFourChars), std::end(lastFourChars), letter) != 1)
                {
                    areAllDistinct = false;
                }
            }

            if (areAllDistinct)
            {
                std::cout << (pos + 1) << std::endl;
                break;
            }
        }

    }

}
