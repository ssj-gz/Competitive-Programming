#include <iostream>
#include <algorithm>

#include <cassert>

using namespace std;

int main()
{
    string directions;
    std::getline(cin, directions);
    const int floor = std::count(directions.begin(), directions.end(), '(') - std::count(directions.begin(), directions.end(), ')');
    std::cout << "floor: " << floor << std::endl;
    return 0;
}
