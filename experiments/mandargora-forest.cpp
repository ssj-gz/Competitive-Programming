#include <iostream>
#include <vector>

using namespace std;

void solveBruteForce(const vector<int>& H, int mandragoraNum, int64_t currentHealth, int64_t currentExperience, int64_t& bestExperience, string& choicesSoFar)
{
    cout << "mandragoraNum: " << mandragoraNum << " choicesSoFar: " << choicesSoFar << " currentHealth: " << currentHealth << " currentExperience: " << currentExperience << endl;
    if (mandragoraNum == H.size())
    {
        if (currentExperience > bestExperience)
        {
            bestExperience = currentExperience;
            cout << "new best experience: " << bestExperience << endl;
        }
        return;
    }

    choicesSoFar.push_back('e');
    solveBruteForce(H, mandragoraNum + 1, currentHealth + 1, currentExperience, bestExperience, choicesSoFar);
    choicesSoFar.pop_back();
    choicesSoFar.push_back('b');
    solveBruteForce(H, mandragoraNum + 1, currentHealth, currentExperience + currentHealth * H[mandragoraNum], bestExperience, choicesSoFar);
    choicesSoFar.pop_back();
}

int64_t solveBruteForce(const vector<int>& originalH)
{
    int64_t bestExperience = 0;
    auto hPermutation = originalH;
    do
    {
        cout << "Trying with permuted health: " << endl;
        for (const auto x : hPermutation)
        {
            cout << " " << x;
        }
        cout << endl;
        int64_t bestExperienceForPermutation = 0;
        string choicesSoFar;
        solveBruteForce(hPermutation, 0, 1, 0, bestExperience, choicesSoFar);

        bestExperience = max(bestExperience, bestExperienceForPermutation);
        next_permutation(hPermutation.begin(), hPermutation.end());

    }
    while (hPermutation != originalH);
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

        const auto solutionBruteForce = solveBruteForce(H);
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
    }



}
