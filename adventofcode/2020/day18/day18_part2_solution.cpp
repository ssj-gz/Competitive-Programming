#include <iostream>
#include <sstream>
#include <vector>
#include <numeric>
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
    vector<string> topLevelValuesAndOps;
    while (expressionStream >> token)
    {
        int tokenBeginPos = expressionStream.tellg() - token.size();
        int tokenEndPos = expressionStream.tellg();
        if (bracketLevel == 0)
        {
            if (token == "+" || token == "*")
            {
                currentOp = token.front();
                topLevelValuesAndOps.push_back(token);
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
                topLevelValuesAndOps.push_back(token);
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
                    topLevelValuesAndOps.push_back(to_string(evaluateExpression(expression.substr(topLevelBracketPos + 1, tokenEndPos - topLevelBracketPos - 2))));
                }
            }

        }
    }
    cout << "expression: " << expression << " top level: " << endl;
    currentOp = '\0';
    vector<string> topLevelValuesAndOps2;
    int previousMultPos = -1;
    result = 1;
    for (int tokenIndex = 0; tokenIndex < topLevelValuesAndOps.size(); tokenIndex++)
    {
        if (topLevelValuesAndOps[tokenIndex] == "*")
        {
            string blah = std::accumulate(topLevelValuesAndOps.begin() + 1 + previousMultPos, topLevelValuesAndOps.begin() + tokenIndex, string());
            blah = getNormalisedExpression(blah);
            cout << "left expression: " << blah << endl;
            const auto leftResult = evaluateExpression(blah);
            result *= leftResult;
            previousMultPos = tokenIndex;
        }
    }
    if (previousMultPos != -1)
    {
        string blah = std::accumulate(topLevelValuesAndOps.begin() + 1 + previousMultPos, topLevelValuesAndOps.end(), string());
        blah = getNormalisedExpression(blah);
        cout << "left expression: " << blah << endl;
        const auto leftResult = evaluateExpression(blah);
        result *= leftResult;
    }
    else
    {
        // There were no top-level +'s, so only +'s and numbers were present.
        result = 0;
        cout << "adding" << endl;
        for (const auto token : topLevelValuesAndOps)
        {
            if (token != "+")
            {
                int64_t value = stol(token);
                result += value;
            }
        }
        cout << " adding result: " << result << endl;
    }
    cout << endl;
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
