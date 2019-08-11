// Simon St James (ssjgz) - 2019-08-03
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>

#include <cassert>

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

bool solveBruteForce(const string& cardsState)
{
    const auto numCards = cardsState.size();
    set<string> visitedCardStates = { cardsState };
    vector<string> cardStatesToExplore = { cardsState };

    while (!cardStatesToExplore.empty())
    {
        vector<string> nextCardStatesToExplore;

        for (const auto& cardState : cardStatesToExplore)
        {
            if (cardState == string(numCards, '.'))
                return true;

            for (auto i = 0; i < numCards; i++)
            {
                if (cardState[i] == '1')
                {
                    string nextCardState = cardState;
                    nextCardState[i] = '.';
                    if (i - 1 >= 0 && nextCardState[i - 1] != '.')
                        nextCardState[i - 1] = '0' + ('1' - nextCardState[i - 1]);
                    if (i + 1 < numCards && nextCardState[i + 1] != '.')
                        nextCardState[i + 1] = '0' + ('1' - nextCardState[i + 1]);


                    if (visitedCardStates.find(nextCardState) == visitedCardStates.end())
                    {
                        visitedCardStates.insert(nextCardState);
                        nextCardStatesToExplore.push_back(nextCardState);
                    }
                }
            }
        }

        cardStatesToExplore = nextCardStatesToExplore;
    }
    return false;
}

bool calcCanChefWin(const string& cardsState)
{
    const auto numFaceUp = std::count(cardsState.begin(), cardsState.end(), '1');
    return ((numFaceUp % 2) == 1);
}

int main(int argc, char* argv[])
{
    const auto T = read<int>();
    for (auto t = 0; t < T; t++)
    {
        const string cardsState = read<string>();

        const auto canChefWin = calcCanChefWin(cardsState);
        cout << (canChefWin ? "WIN" : "LOSE") << endl;
    }
}


