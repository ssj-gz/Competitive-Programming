// Simon St James (ssjgz) - 2019-12-05
// 
// Solution to: https://www.codechef.com/problems/GERALD04
//
#include <iostream>
#include <vector>

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

pair<int, int> calcNumSecondsUntilMeetingForPlans(const int girlfriendBustopTime, const int chefBustopTime, const int secondsToReachHome)
{
    assert(chefBustopTime < girlfriendBustopTime);
    // NB - because we're working in seconds, a half-minute is not a fractional value as it would be if we were
    // working in minutes, so there is no need to use floating point (and all the problems that would bring with it!)
    const int secondsUntilGirlfriendArrives = girlfriendBustopTime - chefBustopTime;
    // Plan 1.
    const int plan1NumSecondsUntilMeeting = secondsUntilGirlfriendArrives + secondsToReachHome;
    {
        // Plan 2.
        int timeForChefToReachHome = secondsToReachHome;
        if (timeForChefToReachHome <= secondsUntilGirlfriendArrives)
        {
            // Chef reaches home before girlfriend arrives, so he's heading back towards busstop.
            int chefDistFromHomeWhenGirlfriendArrivesAtBusStop = secondsUntilGirlfriendArrives - timeForChefToReachHome;
            if (chefDistFromHomeWhenGirlfriendArrivesAtBusStop >= secondsToReachHome)
            {
                // He reaches the bustop before girlfriend arrives, and gives her the present when she does arrive.
                return {plan1NumSecondsUntilMeeting, secondsUntilGirlfriendArrives};
            }
            else
            {
                // Chef is heading back to the bustop when his girlfriend arrives at the bustop; they meet
                // halfway between her position (the bustop, which is secondsToReachHome from home) and his position.
                return {plan1NumSecondsUntilMeeting, secondsUntilGirlfriendArrives + (secondsToReachHome - chefDistFromHomeWhenGirlfriendArrivesAtBusStop) / 2};
            }
        }
        else
        {
            // Chef has not reached home by the time girlfriend arrives.  They will meet halfway between the Home, and the 
            // position the girlfriend is at when Chef finally reaches home.
            const int chefDistFromHomeWhenGirlfriendArrivesAtBusStop = secondsToReachHome - secondsUntilGirlfriendArrives;
            const int girlfriendDistFromHomeWhenChefArrivesHome = secondsToReachHome - chefDistFromHomeWhenGirlfriendArrivesAtBusStop;
            return {plan1NumSecondsUntilMeeting, secondsToReachHome + girlfriendDistFromHomeWhenChefArrivesHome / 2};
        }
    }
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        auto readSecondsFromMidnight = []()
        {
            const string timeHHMM = read<string>();
            const int hour = stoi(timeHHMM);
            const int minute = stoi(timeHHMM.substr(3));

            return hour * 3600 + minute * 60;
        };

        const int girlfriendBustopTime = readSecondsFromMidnight();
        const int chefBustopTime = readSecondsFromMidnight();
        const int secondsToReachHome = read<int>() * 60;

        auto printMinutes = [](const int numSecondsUntilMeeting)
        {
            const int wholeminutesUntilMeeting = numSecondsUntilMeeting / 60;
            const int remainderSecondsUntilMeeting = numSecondsUntilMeeting % 60;
            assert(remainderSecondsUntilMeeting == 30 || remainderSecondsUntilMeeting == 0);

            cout << wholeminutesUntilMeeting << "." << (remainderSecondsUntilMeeting == 0 ? '0' : '5');
        };

        const auto result = calcNumSecondsUntilMeetingForPlans(girlfriendBustopTime, chefBustopTime, secondsToReachHome);
        printMinutes(result.first);
        cout << " ";
        printMinutes(result.second);
        cout << endl;
    }


    assert(cin);
}
