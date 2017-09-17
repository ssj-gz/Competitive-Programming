// Simon St James (ssjgz) 2017-09-17 08:37
#include <iostream>
#include <vector>

using namespace std;

vector<string> findPasswordSequence(const string& loginAttempt, const vector<string>& passwords, vector<string>& sequenceSoFar, int sequenceSoFarNumChars, vector<bool>& isSuffixBlacklisted)
{
    if (sequenceSoFarNumChars == loginAttempt.length())
        return sequenceSoFar;
    if (isSuffixBlacklisted[sequenceSoFarNumChars])
        return vector<string>();

    for (const auto& password : passwords)
    {
        const int newSequenceSoFarNumChars = sequenceSoFarNumChars + password.length();
        if (newSequenceSoFarNumChars <= loginAttempt.length() && !isSuffixBlacklisted[newSequenceSoFarNumChars])
        {
            const bool passwordFits = (loginAttempt.find(password, sequenceSoFarNumChars) == sequenceSoFarNumChars);
            if (passwordFits)
            {
                sequenceSoFar.push_back(password);

                const auto result = findPasswordSequence(loginAttempt, passwords, sequenceSoFar, newSequenceSoFarNumChars, isSuffixBlacklisted);

                sequenceSoFar.pop_back();

                if (!result.empty())
                {
                    // Found!
                    return result;
                }
            }
        }
    }
    // Unsuccessful.
    isSuffixBlacklisted[sequenceSoFarNumChars] = true;
    return vector<string>();
}

int main()
{
    // Very easy one; code is pretty much self-explanatory, I think :)
    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        int N;
        cin >> N;
        vector<string> passwords(N);
        for (int i = 0; i < N; i++)
        {
            cin >> passwords[i];
        }
        string loginAttempt;
        cin >> loginAttempt;

        vector<string> emptySequence;
        vector<bool> isSuffixBlacklisted(loginAttempt.length() + 1, false); // A string of length N has N + 1 suffixes.
        const auto result = findPasswordSequence(loginAttempt, passwords, emptySequence, 0, isSuffixBlacklisted);
        
        if (result.empty())
            cout << "WRONG PASSWORD" << endl;
        else
        {
            for (const auto& password : result)
            {
                cout << password << " ";
            }
            cout << endl;
        }
    }
}

