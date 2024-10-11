#include <iostream>

using namespace std;

int main()
{
    string polymer;
    cin >> polymer;
    std::cout << " initial polymer: " << polymer << std::endl;

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
    std::cout << "polymer: " << polymer << " size: " << polymer.size() << std::endl;
    return 0;
}
