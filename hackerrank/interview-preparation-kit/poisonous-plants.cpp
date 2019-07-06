// Simon St James (ssjgz) - 2019-04-06
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
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
int solveOptimised(const vector<int>& pOrig)
{
    vector<int> p(pOrig);
    struct ElementInfo
    {
        int value = -1;
        int killsAt = 0;
        int killedAt = 0;
    };
    vector<ElementInfo> killerStack;
    int result = 0;

    // Stick the first element on the stack: it hasn't killed yet,
    // and can never be killed.
    killerStack.push_back({p.front(), 0, 0});
    p.erase(p.begin());

    for (const auto x : p)
    {
        //cout << "index: " << index << " x: " << x << " previousValue: " << previousValue << " stackKillsOnDay: " << stackKillsOnDay << " result: " << result << " stack: " << endl;
        //for (const auto s : killerStack)
        //{
            //cout << s.value << "[" << s.killsAt << "," << s.killedAt << "]" << " ";
        //}
        //cout << endl;
        assert(!killerStack.empty());

        while (!killerStack.empty() && x <= killerStack.back().value)
        {
            // We're lower than the current "killer" value, so cannot be killed yet;
            // unwind the stack of all that are now to be killed.
            // Do them in continuous blocks of all elements that die on the same day.
            const int topKilledAt = killerStack.back().killedAt;
            while (!killerStack.empty() && killerStack.back().killedAt == topKilledAt)
            {
                killerStack.pop_back();
            }
        }
        if (!killerStack.empty() && x > killerStack.back().value)
        {
            // Put us on the stack as the new killer, but note that we will be killed
            // soon.
            // Update the previous killer's "killsAt" to reflect when it will kill us.
            killerStack.back().killsAt++;
            result = max(result, killerStack.back().killsAt);
            killerStack.push_back({x, 0, killerStack.back().killsAt});
        }
        else
        {
            // We cannot be killed, and are the new killer.
            killerStack.push_back({x, 0, 0});
        }


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

#ifdef BRUTE_FORCE
    const auto solutionBruteForce = solveBruteForce(p);
    cout << "solutionBruteForce: " << solutionBruteForce << endl;
    const auto solutionOptimised = solveOptimised(p);
    cout << "solutionOptimised: " << solutionOptimised << endl;
    assert(solutionOptimised == solutionBruteForce);
#else
    const auto solutionOptimised = solveOptimised(p);
    cout << solutionOptimised << endl;
#endif
}


