// Simon St James (ssjgz) - 2019-12-05
// 
// Solution to: https://www.codechef.com/problems/GERALD04
//
#include <iostream>
#include <vector>
#include <iomanip>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

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
                // halfway between her position (the busttop, secondsToReachHome from home) and his position.
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
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        // TODO - generate randomised test.
        //const int T = rand() % 100 + 1;
        const int T = 1;
        cout << T << endl;

        for (int t = 0; t < T; t++)
        {
            const int dist = 1 + (rand() % 180);
            auto padToLength2 = [](const int num)
            {
                string numAsString = to_string(num);
                if (numAsString.size() < 2)
                    numAsString = '0' + numAsString;

                return numAsString;
            };

            string time1;
            string time2;
            while (true)
            {
                time1 = padToLength2(rand() % 24) + ":" + padToLength2(rand() % 60);
                time2 = padToLength2(rand() % 24) + ":" + padToLength2(rand() % 60);

                if (time2 > time1)
                    swap(time1, time2);
                if (time1 != time2)
                    break;
            }

            cout << time1 << endl;
            cout << time2 << endl;
            cout << dist << endl;
        }

        return 0;
    }
    
    const auto T = read<int>();

    cout << fixed;
    cout << setprecision(1);

    for (int t = 0; t < T; t++)
    {
        auto readSecondsFromMidnight = []()
        {
            const string timeHHMM = read<string>();
            const int hour = stoi(timeHHMM);
            const int minute = stoi(timeHHMM.substr(3));
            //cout << "hour: " << hour << " minute:" << minute << endl;

            return hour * 3600 + minute * 60;
        };

        const int girlfriendBustopTime = readSecondsFromMidnight();
        const int chefBustopTime = readSecondsFromMidnight();
        assert(chefBustopTime < girlfriendBustopTime);
        const int secondsToReachHome = read<int>() * 60;

        auto printMinutes = [](const int numSecondsUntilMeeting)
        {
            const int wholeminutesUntilMeeting = numSecondsUntilMeeting / 60;
            const int remainderSecondsUntilMeeting = numSecondsUntilMeeting % 60;
            assert(remainderSecondsUntilMeeting == 30 || remainderSecondsUntilMeeting == 0);
            //cout << "remainderSecondsUntilMeeting: " << remainderSecondsUntilMeeting << endl;

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
