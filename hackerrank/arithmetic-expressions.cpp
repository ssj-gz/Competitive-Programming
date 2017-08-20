// Simon St James (ssjgz) - 2017-08-20 16:39
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

int main()
{
    int N;
    cin >> N;
    vector<int> numbers(N);
    assert(N >= 2);
    for (int i = 0; i < N; i++)
    {
        cin >> numbers[i];
        assert(numbers[i] <= 100);
        assert(numbers[i] >= 1);
    }
    assert(cin);
    const char operatorChars[] = { '+', '-', '*' };
    const int numOperators = sizeof(operatorChars) / sizeof(operatorChars[0]);
    const int m = 101;

    // operatorCharToReachValue[x][y] is the last operator used to obtain the 
    // value y (modulo m) using the first x numbers (or null char if we cannot form y modulo m using
    // the first x numbers).
    vector<vector<char>> operatorCharToReachValue;
    operatorCharToReachValue.push_back(vector<char>(m, '\0'));
    operatorCharToReachValue[0][numbers[0]] = '+';
    //cout << "Bleep: " << operatorCharToReachValue[0][0] << endl;

    for (int numberIndex = 1; numberIndex < N; numberIndex++)
    {
        const auto number = numbers[numberIndex];
        //cout << "number: " << number << endl;
        vector<char> newOperatorCharToReachValue(m, '\0');
        const vector<char>& operatorCharToReachLastValue = operatorCharToReachValue.back();
        for (int lastValue = 0; lastValue < m; lastValue++)
        {
            //cout << " last Value: " << lastValue << endl;
            if (operatorCharToReachLastValue[lastValue] != '\0')
            {
                //cout << " is reachable" << endl;
                // Add number.
                newOperatorCharToReachValue[(lastValue + number) % m] = '+';
                // Subtract number.
                newOperatorCharToReachValue[(lastValue + m - number) % m] = '-';
                // Multiply by number.
                newOperatorCharToReachValue[(lastValue * number) % m] = '*';
            }
        }
        operatorCharToReachValue.push_back(newOperatorCharToReachValue);
    }
    //cout << "thing" << endl;
    int currentValue = 0;
    string expression;
    assert(operatorCharToReachValue.size() == N);
    for (int numberIndex = N - 1; numberIndex >= 0; numberIndex--)
    {
        //cout << " numberIndex: " << numberIndex << " number: " << numbers[numberIndex] << " currentValue: " << currentValue << endl;
        const int number = numbers[numberIndex];
        expression = to_string(number) + expression;
        //cout << " operator: " << operatorCharToReachValue[numberIndex][currentValue] << endl;
        assert(operatorCharToReachValue[numberIndex][currentValue] != '\0');
        if (numberIndex != 0)
            expression = operatorCharToReachValue[numberIndex][currentValue] + expression;
        switch (operatorCharToReachValue[numberIndex][currentValue])
        {
            case '+':
                currentValue = currentValue + m - number;
                break;
            case '-':
                currentValue = currentValue + number;
                break;
            case '*':
                //cout << " multiplication; searching ... " << endl;
                for (int i = 0; i < m; i++)
                {
                    //cout << " i: " << i << " i * number: " << i * number << endl;
                    if ((i * number) % m == currentValue)
                    {
                        currentValue = i;
                        //cout << " found" << endl;
                        break;
                    }
                }
                break;
        }
        currentValue %= m;
    }
    cout << expression << endl;
}
