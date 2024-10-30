#include <iostream>
#include <sstream>
#include <limits>

#include <cassert>

using namespace std;

int main()
{
    string rowNumbersString;
    int64_t checksum = 0;
    while (getline(cin, rowNumbersString))
    {
        std::cout << "row: " << rowNumbersString << std::endl;
        int64_t min = std::numeric_limits<int64_t>::max();
        int64_t max = std::numeric_limits<int64_t>::min();
        istringstream rowNumbersStream(rowNumbersString);
        int64_t number;
        while (rowNumbersStream >> number)
        {
            min = std::min(min, number);
            max = std::max(max, number);
        }
        checksum += max - min;
    }
    std::cout << "checksum: " << checksum << std::endl;
    return 0;
}
