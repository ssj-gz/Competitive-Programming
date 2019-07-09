// Simon St James (ssjgz) - 2019-07-06
#include <iostream>
#include <vector>
#include <stack>

#include <cassert>

using namespace std;

int findLastKillingDay(const vector<int>& pOrig)
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

    // Stick the first element on the stack and remove it from p. 
    // It hasn't killed yet, and can never be killed.
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
    // Hmmm ... took me quite a while to figure out, and am finding it very hard to 
    // explain it :/
    //
    // Clearly, *increasing* runs are of the utmost importance here, since the plants
    // in this runs (except for the first in the run) will all die on the same day,
    // and removing a run can induce another run, the elements of which will be killed
    // on a subsequent day - so runs can "nest" inside of each other, hinting that a 
    // stack-based approach might be the way to go.
    //
    // So for each element, we give it a "killsAtDay" and "killedAtDay" - the result is the
    // maximum value over all elements of "killedAtDay".  
    //
    // Damn - this is really tough to explain, but hopefully the code gives some hints.
    // Might come back and try and come up with a better explanation later :/
    int n;
    cin >> n;

    vector<int> p(n);

    for (int i = 0; i < n; i++)
    {
        cin >> p[i];
    }

    cout << findLastKillingDay(p) << endl;
}


