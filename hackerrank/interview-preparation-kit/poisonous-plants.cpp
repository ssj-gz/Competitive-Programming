#include <iostream>
#include <vector>

#include <list>

#include <sys/time.h>

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
}


