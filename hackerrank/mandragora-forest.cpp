// Simon St James (ssjgz) - 2019-05-21
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int64_t findBestExperience(const vector<int>& originalH)
{
    const int n = originalH.size();

    int64_t bestExperience = 0;
    auto sortedH = originalH;
    sort(sortedH.begin(), sortedH.end());

    vector<int64_t> sumOfRemainingHealths(n + 1);
    int64_t sumOfLastHealths = 0;
    for (const auto h : sortedH)
    {
        sumOfLastHealths += h;
    }
    for (int numRemaining = n; numRemaining >= 0; numRemaining--)
    {
        sumOfRemainingHealths[numRemaining] = sumOfLastHealths;
        if (n - numRemaining >= 0)
            sumOfLastHealths -= sortedH[n - numRemaining];
    }

    int64_t currentHealthIfOnlyEat = 1;
    const int64_t currentExperienceIfOnlyEat = 0;
    for (int i = 0; i < n; i++)
    {
        const int64_t currentExperience = currentHealthIfOnlyEat * sumOfRemainingHealths[n - i];
        bestExperience = max(bestExperience, currentExperience);
        currentHealthIfOnlyEat++;
    }
    return bestExperience;
}

int main(int argc, char* argv[])
{
    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        int n;
        cin >> n;

        vector<int> H(n);
        for (int i = 0; i < n; i++)
        {
            cin >> H[i];
        }

        cout << findBestExperience(H) << endl;
    }
}
