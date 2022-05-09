#include <iostream>
#include <vector>
#include <iterator>

using namespace std;

int main()
{
    string inputString;
    cin >> inputString;

    vector<int> input;
    for (const auto digit : inputString)
        input.push_back(digit - '0');

    const int inputLength = static_cast<int>(input.size());

    const int basePattern[] = {0, 1, 0, -1};

    int phaseNum = 0;
    while (true)
    {
        vector<int> output;
        for (int outputIndex = 0; outputIndex < inputLength; outputIndex++)
        {
            const int timesToRepeat = outputIndex + 1;
            int countdownToNextBase = timesToRepeat - 1;
            int baseIndex = 0;
            int outputVal = 0;
            for (int i = 0; i < inputLength; i++)
            {
                if (countdownToNextBase == 0)
                {
                    baseIndex = (baseIndex + 1) % size(basePattern);
                    countdownToNextBase = timesToRepeat;
                }
                //cout << "i: " << i << " baseIndex: " << baseIndex << " basePattern[baseIndex]: " << basePattern[baseIndex] << " input[i]:" << input[i] << " countdownToNextBase: " << countdownToNextBase << endl;
                outputVal = outputVal + basePattern[baseIndex] * input[i];

                countdownToNextBase--;
            }
            outputVal = abs(outputVal) % 10;
            //cout << "outputIndex: " << outputIndex << " outputVal:  " << outputVal << endl;
            output.push_back(outputVal);
        }
        phaseNum++;
        cout << "After " << phaseNum << " phases:" << endl;
        for (const auto x : output)
            cout << x;
        cout << endl;

        cout << "First 8: " << endl;
        for (int i = 0; i < 8; i++)
            cout << output[i];
        cout << endl;
        if (phaseNum == 100)
            break;

        input = output;
    }
}
