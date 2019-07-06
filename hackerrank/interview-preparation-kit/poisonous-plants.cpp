// Simon St James (ssjgz) - 2019-04-06
#include <iostream>
#include <vector>
#include <stack>

#include <cassert>

using namespace std;

int solveOptimised(const vector<int>& pOrig)
{
    vector<int> p(pOrig);
    struct ElementInfo
    {
        int value = -1;
        int killsAtDay = 0;
        int killedAtDay = 0;
    };
    stack<ElementInfo> killerStack;
    int latestKillingDay = 0;

    // Stick the first element on the stack: it hasn't killed yet,
    // and can never be killed.
    killerStack.push({p.front(), 0, 0});
    p.erase(p.begin());

    for (const auto x : p)
    {
        assert(!killerStack.empty());

        while (!killerStack.empty() && x <= killerStack.top().value)
        {
            // We're lower than the current "killer" value, so cannot be killed yet;
            // unwind the stack of all that are now to be killed.
            // Do them in continuous blocks of all elements that die on the same day.
            const int topKilledAt = killerStack.top().killedAtDay;
            while (!killerStack.empty() && killerStack.top().killedAtDay == topKilledAt)
            {
                killerStack.pop();
            }
        }
        if (!killerStack.empty() && x > killerStack.top().value)
        {
            // Put us on the stack as the new killer, but note that we will be killed
            // soon by the previous killer.
            // Update the previous killer's "killsAtDay" to reflect when it will kill us.
            killerStack.top().killsAtDay++;
            const auto previousKillerKillsAtDay = killerStack.top().killsAtDay;
            latestKillingDay = max(latestKillingDay, previousKillerKillsAtDay);
            killerStack.push({x, 0, previousKillerKillsAtDay});
        }
        else
        {
            // We cannot be killed, and are the new killer.
            killerStack.push({x, 0, 0});
        }
    }
    return latestKillingDay;
}

int main(int argc, char* argv[])
{
    int n;
    cin >> n;

    vector<int> p(n);

    for (int i = 0; i < n; i++)
    {
        cin >> p[i];
    }

    cout << solveOptimised(p) << endl;
}


