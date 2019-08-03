// Simon St James (ssjgz) - 2019-08-03
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>

#include <cassert>

#include <sys/time.h>

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
    const int numCards = cardsState.size();
    set<string> visitedCardStates = { cardsState };
    vector<string> cardStatesToExplore = { cardsState };

    while (!cardStatesToExplore.empty())
    {
        vector<string> nextCardStatesToExplore;

        for (const auto& cardState : cardStatesToExplore)
        {
            if (cardState == string(numCards, '.'))
                return true;

            for (int i = 0; i < numCards; i++)
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

int main(int argc, char* argv[])
{
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        const int numCards = rand() % 13 + 1;
        string cardsState;
        for (int i = 0; i < numCards; i++)
        {
            cardsState.push_back('0' + rand() % 2);
        }
        cout << 1 << endl;
        cout << cardsState << endl;
        return 0;
    }

    const int T = read<int>();
    for (int t = 0; t < T; t++)
    {
        const string cardsState = read<string>();

        const auto solutionBruteForce = solveBruteForce(cardsState);
        cout << "solutionBruteForce: " << (solutionBruteForce ? "WIN" : "LOSE") << endl;

        const int numOn = std::count(cardsState.begin(), cardsState.end(), '1');
        const int numOff = std::count(cardsState.begin(), cardsState.end(), '0');
        if (!solutionBruteForce)
        {
            cout << "sausage: " << cardsState << " numOn: " << numOn << " numOff: " << numOff << endl;
            assert(numOn % 2 == 0);
        }
        else
        {
            assert(numOn % 2 == 1);
        }
    }
}


