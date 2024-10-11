#include <iostream>
#include <algorithm>
#include <limits>

using namespace std;

int main()
{
    string initialPolymer;
    cin >> initialPolymer;
    std::cout << " initial polymer: " << initialPolymer << std::endl;

    int shortestReactedPolymerLength = std::numeric_limits<int>::max();
    for (auto polymerLowercase = 'a'; polymerLowercase <= 'z'; polymerLowercase++)
    {
        string polymer = initialPolymer;
        polymer.erase(std::remove(polymer.begin(), polymer.end(), static_cast<char>(polymerLowercase)), polymer.end());
        polymer.erase(std::remove(polymer.begin(), polymer.end(), std::toupper(polymerLowercase)), polymer.end());

        while (true)
        {
            bool reactionOccured = false;
            for (auto polymerLeftPos = polymer.begin(); polymerLeftPos != polymer.end(); polymerLeftPos++)
            {
                const auto polymerRightPos = std::next(polymerLeftPos);
                if (polymerRightPos != polymer.end())
                {
                    if ((std::tolower(*polymerLeftPos) == std::tolower(*polymerRightPos)) && (*polymerLeftPos != *polymerRightPos))
                    {
                        reactionOccured = true;
                        polymerLeftPos = polymer.erase(polymerLeftPos);
                        polymerLeftPos = polymer.erase(polymerLeftPos);
                        std::cout << " reaction occurred; polymer size becomes " << polymer.size() << std::endl;
                        break;
                    }
                }
            }


            if (!reactionOccured)
                break;
        }
        std::cout << "polymerLowercase: " << polymerLowercase << " size: " << polymer.size() << std::endl;
        shortestReactedPolymerLength = std::min<int>(shortestReactedPolymerLength, polymer.size());
    }
    std::cout << "shortestReactedPolymerLength: " << shortestReactedPolymerLength << std::endl;
    return 0;
}
