#include <iostream>
#include <vector>
#include <iterator>
#include <cassert>

using namespace std;

int main()
{
    string inputString;
    cin >> inputString;

    vector<int> stemInput;
    for (const auto digit : inputString)
        stemInput.push_back(digit - '0');

    // Repeat output 10'000 times, as requested.
    vector<int> input;
    for (int i = 0 ; i < 10'000; i++)
    {
        input.insert(input.end(), stemInput.begin(), stemInput.end());
    }

    int64_t offset = 0;
    for (int i = 0; i < 7; i++)
    {
        offset = offset * 10 + input[i];
    }

    const int inputLength = static_cast<int>(input.size());

    const int basePattern[] = {0, 1, 0, -1};

    int phaseNum = 0;
    while (true)
    {
        vector<int> output;
        output.reserve(inputLength);

        vector<int> prefixSum;
        prefixSum.reserve(inputLength + 1);
        int sum = 0;
        prefixSum.push_back(sum);
        for (const auto x : input)
        {
            sum += x ;
            prefixSum.push_back(sum);
        }

        for (int outputIndex = 0; outputIndex < inputLength; outputIndex++)
        {
            const int timesToRepeat = outputIndex + 1;

            // We'll basically be adding (larger and larger) blocks of
            // consecutive integers from "input", all multiplied by
            // basePattern[baseIndex], which is always 0, -1 or 1.
            // Use prefix sums to perform the addition of these blocks.
            int i = 0;
            int blockSize = timesToRepeat - 1; // Arrange to drop the very first repetition of 
                                               // the base pattern, as requested.
            int baseIndex = 0;
            int outputVal = 0;
            while (i < inputLength)
            {
                const int amountToAdd = prefixSum[min(i + blockSize, inputLength)] - prefixSum[i];

                switch(basePattern[baseIndex])
                {
                    case 0:
                        break;
                    case 1:
                        outputVal += amountToAdd;
                        break;
                    case -1:
                        outputVal -= amountToAdd;
                        break;
                }
                i += blockSize;
                // Done this block, all multiplied by the basePattern[baseIndex];
                // move onto the next block, which will use the next base pattern.
                baseIndex = (baseIndex + 1) % size(basePattern);
                // Drop no more repetitions of the base pattern.
                blockSize = timesToRepeat;
            }

            output.push_back(abs(outputVal) % 10);
        }
        phaseNum++;
        cout << "After " << phaseNum << " phases:" << endl;

        cout << "First 8: " << endl;
        for (int i = 0; i < 8; i++)
            cout << output[i];
        cout << endl;
        if (phaseNum == 100)
        {
            cout << "Done; offset: " << offset << " output len: " << output.size() << endl;
            for (auto answerDigitIter = output.begin() + offset; answerDigitIter != output.begin() + offset + 8; answerDigitIter++)
            {
                cout << *answerDigitIter;
            }
            cout << endl;
            break;
        }

        input = output;
    }
}
