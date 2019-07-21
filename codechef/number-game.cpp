#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main()
{
    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {

        string A;
        cin >> A;

        int modulus;
        cin >> modulus;

        int numStartingMoves = 0;
        for (int i = 0; i < A.size(); i++)
        {
            string aMinusDigit = A.substr(0, i) + A.substr(i + 1);
            const int aMinusDigitMod = stoi(aMinusDigit) % modulus;

            vector<int> digitsInAMinusDigit;
            for (int digit = 0; digit <= 9; digit++)
            {
                if (A.find('0' + digit) != string::npos)
                    digitsInAMinusDigit.push_back('0' + digit);
            }

            vector<bool> processed(modulus, false);
            vector<int> toProcess = { aMinusDigitMod };

            while (!toProcess.empty())
            {
                //cout << " # toProcess: " << toProcess.size() << endl;
                vector<int> nextToProcess;
                for (const auto p : toProcess)
                {
                    processed[p] = true;
                    int pTimes10 = (p * 10) % modulus;
                    for (const auto digit : digitsInAMinusDigit)
                    {
                        const int newValue = (pTimes10 + digit) % modulus;
                        if (!processed[newValue])
                        {
                            nextToProcess.push_back(newValue);
                        }
                    }
                }
                toProcess = nextToProcess;
            }

            if (processed[0])
                numStartingMoves++;

        }
        cout << numStartingMoves << endl;



    }
}
