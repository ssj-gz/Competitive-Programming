#include <iostream>
#include <algorithm>
#include <cassert>

using namespace std;

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
        vector<string> onWires;
        for (int i = 0; i < 10; i++)
        {

            string onWire;
            cin >> onWire;
            onWires.push_back(onWire);
            cout << "onWire: " << onWire << endl;
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
            sort(displayDigit.begin(), displayDigit.end());
        }
        assert(cin);

        const string originalPermutation = "abcdefg";
        string permutation = originalPermutation;
        do
        {
            next_permutation(permutation.begin(), permutation.end());
            //cout << "permutation: " << permutation << endl;
            bool isDigitUsed[allDigits.size()] = {};
            int digitSegmentForWireIndex[allDigits.size()] = {};
            int numDigitsUsed = 0;
            for (int wireIndex = 0; wireIndex < allDigits.size(); wireIndex++)
            {
                const auto& onWire = onWires[wireIndex];
                string candidateOnSegments;
                for (const auto& wire : onWire)
                {
                    const char digitSegment = permutation[wire - 'a'];
                    candidateOnSegments.push_back(digitSegment);
                }
                sort(candidateOnSegments.begin(), candidateOnSegments.end());
                //cout << "candidateOnSegments: " << candidateOnSegments << endl;

                for (int digitIndex = 0; digitIndex < allDigits.size(); digitIndex++)
                {
                    if (allDigits[digitIndex] == candidateOnSegments && !isDigitUsed[digitIndex])
                    {
                        isDigitUsed[digitIndex] = true;
                        digitSegmentForWireIndex[wireIndex] = digitIndex;
                        numDigitsUsed++;
                    }
                }
                //cout << "numDigitsUsed: " << numDigitsUsed << endl;
                if (numDigitsUsed == allDigits.size())
                {
                    cout << "Found: Display digits: " << endl;

                    string outputDigits;
                    for (const auto& displayDigitWires : display)
                    {
                        string displayDigit;
                        for (const auto& wire : displayDigitWires)
                        {
                            const char digitSegment = permutation[wire - 'a'];
                            displayDigit.push_back(digitSegment);
                        }
                        sort(displayDigit.begin(), displayDigit.end());

                        for (int digitIndex = 0; digitIndex < allDigits.size(); digitIndex++)
                        {
                            if (allDigits[digitIndex] == displayDigit)
                            {
                                outputDigits.push_back('0' + digitIndex);
                            }
                        } 
                    }
                    cout << outputDigits << endl;
                    assert(outputDigits.size() == 4);
                    result += count(outputDigits.begin(), outputDigits.end(), '1');
                    result += count(outputDigits.begin(), outputDigits.end(), '4');
                    result += count(outputDigits.begin(), outputDigits.end(), '7');
                    result += count(outputDigits.begin(), outputDigits.end(), '8');
                    cout << endl;
                }
            }
        } 
        while (permutation != originalPermutation);
    }
    cout << result << endl;
}

