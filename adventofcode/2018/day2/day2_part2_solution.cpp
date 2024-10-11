#include <iostream>
#include <vector>

#include <cassert>

using namespace std;

int main()
{
    vector<string> ids;
    string id;
    while (cin >> id)
    {
        ids.push_back(id);
    }

    for (auto id1Iter = ids.begin(); id1Iter != ids.end(); id1Iter++)
    {
        for (auto id2Iter = std::next(id1Iter); id2Iter != ids.end(); id2Iter++)
        {
            const auto& id1 = *id1Iter;
            const auto& id2 = *id2Iter;
            assert(id1.size() == id2.size());
            int numDifferentLetters = 0;
            int differingLetterPos = -1;
            for (int pos = 0; pos < id1.size(); pos++)
            {
                if (id1[pos] != id2[pos])
                {
                    numDifferentLetters++;
                    differingLetterPos = pos;
                }
            }
            if (numDifferentLetters  == 1)
            {
                auto commonLetters = id1;
                commonLetters.erase(commonLetters.begin() + differingLetterPos);
                std::cout << "result: " << commonLetters << std::endl;
            }

        }
    }

    return 0;
}
