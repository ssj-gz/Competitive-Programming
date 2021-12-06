#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <numeric>

using namespace std;

int main()
{
    string countdownsLine;
    getline(cin, countdownsLine);
    std::replace(countdownsLine.begin(), countdownsLine.end(), ',', ' ');
    istringstream countdownsStream(countdownsLine);

    vector<int64_t> listOfLanternFishCountdowns;
    int64_t countdown = -1;
    while (countdownsStream >> countdown)
    {
        listOfLanternFishCountdowns.push_back(countdown);
    }
    cout << "# blah" << listOfLanternFishCountdowns.size() << endl;

    const int maxCountdown = 8;
    vector<int64_t> numFishWithCountdown(maxCountdown + 1, 0);
    for (const auto countdown : listOfLanternFishCountdowns)
    {
        numFishWithCountdown[countdown]++;
    }

    for (int day = 1; day <= 256; day++)
    {
        vector<int64_t> nextNumFishWithCountdown(maxCountdown + 1, 0);
        for (int countdown = 0; countdown <= maxCountdown; countdown++)
        {
            if (countdown == 0)
            {
                nextNumFishWithCountdown[6] += numFishWithCountdown[countdown];
                nextNumFishWithCountdown[8] += numFishWithCountdown[countdown];
            }
            else
            {
                nextNumFishWithCountdown[countdown - 1] += numFishWithCountdown[countdown];
            }

        }

        numFishWithCountdown = nextNumFishWithCountdown;
        cout << "Day " << day << endl;
#if 0
        for (const auto age : listOfLanternFishCountdowns)
        {
            cout << age << " ";
        }
#endif
        cout << "(total: " << std::accumulate(numFishWithCountdown.begin(), numFishWithCountdown.end(), static_cast<int64_t>(0)) << ")";
        cout << endl;
    }


}
