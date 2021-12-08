#include <iostream>
#include <algorithm>
#include <cassert>

using namespace std;

string applyPermutationAndNormalise(const string& original, const string& permutation)
{
    string result;
    for (const auto& wire : original)
    {
        const char digitSegment = permutation[wire - 'a'];
        result.push_back(digitSegment);
    }
    // Normalise.
    sort(result.begin(), result.end());
    return result;
}

int getMatchingDigitIndex(const string& digitSegments, const vector<string>& allDigits)
{
    for (int digitIndex = 0; digitIndex < allDigits.size(); digitIndex++)
    {
        if (allDigits[digitIndex] == digitSegments)
        {
            return digitIndex;
        }
    } 
    return -1;
}

int main()
{
    const string digit0 = "abcefg";
    const string digit1 = "cf";
    const string digit2 = "acdeg";
    const string digit3 = "acdfg";
    const string digit4 = "bcdf";
    const string digit5 = "abdfg";
    const string digit6 = "abdefg";
    const string digit7 = "acf";
    const string digit8 = "abcdefg";
    const string digit9 = "abcdfg";
    vector<string> allDigits =
    {
        digit0,
        digit1,
        digit2,
        digit3,
        digit4,
        digit5,
        digit6,
        digit7,
        digit8,
        digit9
    };

    for (const auto& digit : allDigits)
    {
        assert(is_sorted(digit.begin(), digit.end()));
    }

    int result = 0;
    while (true)
    {
        vector<string> onWiresList;
        for (int i = 0; i < 10; i++)
        {

            string onWire;
            cin >> onWire;
            onWiresList.push_back(onWire);
        }
        if (!cin)
            break;
        char dummyChar;
        cin >> dummyChar;
        assert(dummyChar == '|');

        vector<string> display(4);
        for (auto& displayDigit: display)
        {
            cin >> displayDigit;
            // Normalise.
            sort(displayDigit.begin(), displayDigit.end());
        }
        assert(cin);

        const string originalPermutation = "abcdefg";
        string permutation = originalPermutation;
        do
        {
            next_permutation(permutation.begin(), permutation.end());
            bool isDigitMatched[allDigits.size()] = {};
            int numDigitsMatched = 0;
            int digitSegmentForWireIndex[allDigits.size()] = {};
            for (int wireIndex = 0; wireIndex < allDigits.size(); wireIndex++)
            {
                const auto& onWires = onWiresList[wireIndex];
                const string digitSegmentsForOnWires = applyPermutationAndNormalise(onWires, permutation);

                const int matchingDigitIndex = getMatchingDigitIndex(digitSegmentsForOnWires, allDigits);
                if (matchingDigitIndex != -1 && !isDigitMatched[matchingDigitIndex])
                {
                    isDigitMatched[matchingDigitIndex] = true;
                    digitSegmentForWireIndex[wireIndex] = matchingDigitIndex;
                    numDigitsMatched++;
                }

                if (numDigitsMatched == allDigits.size())
                {
                    cout << "Found: Display digits: " << endl;

                    string outputDigits;
                    for (const auto& displayDigitWires : display)
                    {
                        const string displayDigitSegments = applyPermutationAndNormalise(displayDigitWires, permutation);
                        const int matchingDigitIndex = getMatchingDigitIndex(displayDigitSegments, allDigits);
                        assert(matchingDigitIndex != -1);
                        outputDigits.push_back('0' + matchingDigitIndex);
                    }
                    cout << outputDigits << endl;
                    assert(outputDigits.size() == 4);
                    result += stol(outputDigits);
                    cout << endl;
                }
            }
        } 
        while (permutation != originalPermutation);
    }
    cout << result << endl;
}

