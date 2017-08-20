// Simon St James (ssjgz) - 2017-08-20 16:39
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

int main()
{
    // This is under the "Recursion" section, and the most natural way of doing it is via recursion,
    // but where's the fun in that? ;)
    // Here's an iterative solution: basically successively incorporates each number and works out
    // the set of values (modulo m = 101) that can be generated using this and the previous
    // numbers, and stores the last arithmetic operation used to generate each value.
    // Once we're processed all numbers in this way, we just "backtrack" through the 
    // values generated (starting at 0 - i.e. divisible by m = 101) and the arithmetic operations used 
    // and form the actual expression.
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
    const auto invalidOp = '\0';
    const int m = 101;

    // The value of operatorCharToReachValue[x][y] is the last operator used to obtain the 
    // value y (modulo m) using the first x-1 numbers (or null char if we cannot form y modulo m using
    // the first x-1 numbers).
    vector<vector<char>> operatorCharToReachValue;
    operatorCharToReachValue.push_back(vector<char>(m, invalidOp));
    operatorCharToReachValue[0][numbers[0]] = '+'; // We can form the first number using a dummy op (pick '+', arbitrarily!).

    for (int numberIndex = 1; numberIndex < N; numberIndex++)
    {
        const auto number = numbers[numberIndex];
        vector<char> newOperatorCharToReachValue(m, invalidOp);
        const vector<char>& operatorCharToReachLastValue = operatorCharToReachValue.back();
        for (int lastValue = 0; lastValue < m; lastValue++)
        {
            if (operatorCharToReachLastValue[lastValue] != invalidOp)
            {
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
    // Backtrack through the operations needed to form the desired "current value" (mod m) - 0, in this case,
    // as we want the value to be divisible by m.
    int currentValue = 0;
    string expression;
    for (int numberIndex = N - 1; numberIndex >= 0; numberIndex--)
    {
        const int number = numbers[numberIndex];
        expression = to_string(number) + expression;
        assert(operatorCharToReachValue[numberIndex][currentValue] != invalidOp && "There's supposed to always be a solution!");
        if (numberIndex != 0) // Don't print the initial, "dummy" op.
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
                // "Divide" currentValue by number XD
                for (int i = 0; i < m; i++)
                {
                    if ((i * number) % m == currentValue)
                    {
                        currentValue = i;
                        break;
                    }
                }
                break;
        }
        currentValue %= m;
    }
    cout << expression << endl;
}
