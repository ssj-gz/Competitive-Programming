#include <iostream>
#include <sstream>
#include <vector>
#include <cassert>

using namespace std;

string getNormalisedExpression(const string& expression)
{
    string normalised;
    for (int i = 0; i < expression.size(); i++)
    {
        switch(expression[i])
        {
            case '*':
            case '+':
            case '(':
            case ')':
                if (!normalised.empty() && normalised.back() != ' ')
                    normalised += ' ';
                normalised += expression[i];
                normalised += ' ';
                break;
            default:
                normalised += expression[i];

        }
    }
    return normalised;
}

int64_t evaluateExpression(const string& expression)
{
    cout << "evaluateExpression: " << expression << endl;
    char currentOp = '\0';
    istringstream expressionStream(expression);
    string token;
    int bracketLevel = 0;
    int topLevelBracketPos = -1;
    int64_t result = -1;


    auto incorporateValue = [&result, &currentOp](int64_t value)
    {
        if (currentOp == '\0')
            result = value;
        else if (currentOp == '*')
            result *= value;
        else if (currentOp == '+')
            result += value;
        else
            assert(false);
    };

    while (expressionStream >> token)
    {
        int tokenBeginPos = expressionStream.tellg() - token.size();
        int tokenEndPos = expressionStream.tellg();
        if (bracketLevel == 0)
        {
            if (token == "+" || token == "*")
            {
                currentOp = token.front();
            }
            else if (token == "(")
            {
                bracketLevel++;
                topLevelBracketPos = tokenBeginPos;
            }
            else
            {
                // Integer, one assumes.
                cout << "token is integer?" << token << endl;
                int64_t value = stol(token);
                incorporateValue(value);
            }
        }
        else
        {
            if (token == "(")
            {
                bracketLevel++;
            }
            else if (token == ")")
            {
                bracketLevel--;
                if (bracketLevel == 0)
                {
                    incorporateValue(evaluateExpression(expression.substr(topLevelBracketPos + 1, tokenEndPos - topLevelBracketPos - 2)));
                }
            }

        }
    }
    assert(bracketLevel == 0);

    return result;
}

int main()
{
    string expression;
    int64_t sumOfResults = 0;
    while (getline(cin, expression))
    {
        expression = getNormalisedExpression(expression);
        const int64_t expressionResult = evaluateExpression(expression);
        cout << "expression: " << expression << " result: " << expressionResult << endl;
        sumOfResults += expressionResult;
    }
    cout << sumOfResults << endl;

}
