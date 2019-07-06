#include <iostream>
#include <vector>
#include <stack>

#include <list>
#include <sys/time.h>
#include <cassert>

using namespace std;

int solveBruteForce(const vector<int>& pVec)
{
    list<int> p(pVec.begin(), pVec.end());
    int day = 1;

    while (true)
    {
        cout << "Iteration; day: " << day << endl;
        cout << "p: " << endl;
        for (const auto x : p)
        {
            cout << x << " ";
        }
        cout << endl;
        list<list<int>::iterator> pItersToKill;

        if (pVec.size() == 1)
            break;

        list<int>::iterator pIter = p.begin();
        pIter++;

        while (pIter != p.end())
        {
            if (*pIter > *std::prev(pIter))
            {
                pItersToKill.push_back(pIter);
            }
            pIter++;
        }

        if (pItersToKill.empty())
            break;

        for (auto pIterToKill : pItersToKill)
        {
            p.erase(pIterToKill);
        }


        day++;
    }

    return day - 1;
}
int solveOptimised(const vector<int>& p)
{
    struct ElementInfo
    {
        int value = -1;
        int killsAt = 0;
        int killedAt = 0;
    };
    vector<ElementInfo> blah;
    int previousValue = -1;
    int stackKillsOnDay = 1;
    int result = 0;

    int index = 0;
    for (const auto x : p)
    {
        cout << "index: " << index << " x: " << x << " previousValue: " << previousValue << " stackKillsOnDay: " << stackKillsOnDay << " result: " << result << " stack: " << endl;
        for (const auto s : blah)
        {
            cout << s.value << "[" << s.killsAt << "," << s.killedAt << "]" << " ";
        }
        cout << endl;

        if (blah.empty())
        {
            blah.push_back({x, 0, 0});
        }
        else
        {
            while (!blah.empty() && x <= blah.back().value)
            {
                const int topKilledAt = blah.back().killedAt;
                cout << " removing killed at from stack : " << topKilledAt << endl;
                while (!blah.empty() && blah.back().killedAt == topKilledAt)
                {
                    blah.pop_back();
                }
            }
            if (!blah.empty() && x > blah.back().value)
            {
                blah.back().killsAt++;
                result = max(result, blah.back().killsAt);
                blah.push_back({x, 0, blah.back().killsAt});
            }
            else
            {
                blah.push_back({x, 0, 0});
            }

        }
        previousValue = x;
        index++;

    }
    return result;
}

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int n = rand() % 1000 + 1;
        const int maxP = rand() % 1000 + 1;

        cout << n << endl;

        for (int i = 0; i < n; i++)
        {
            cout << ((rand() % maxP) + 1) << " ";
        }
        cout << endl;

        return 0;
    }
    int n;
    cin >> n;

    vector<int> p(n);

    for (int i = 0; i < n; i++)
    {
        cin >> p[i];
    }

    const auto solutionBruteForce = solveBruteForce(p);
    cout << "solutionBruteForce: " << solutionBruteForce << endl;
    const auto solutionOptimised = solveOptimised(p);
    cout << "solutionOptimised: " << solutionOptimised << endl;
    assert(solutionOptimised == solutionBruteForce);
}


