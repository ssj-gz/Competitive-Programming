#include <iostream>
#include <vector>

#include <list>

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

int main()
{
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


