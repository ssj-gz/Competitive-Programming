// Simon St James (ssjg) - 2019-04-05
#include <iostream>
#include <stack>

#include <cassert>

using namespace std;

bool isBalanced(const string& bracketString)
{
    std::stack<char> bracketStack;

    for (const auto bracket : bracketString)
    {
        if (bracket == '{' || bracket == '[' || bracket == '(')
        {
            // Opening bracket.
            bracketStack.push(bracket);
        }
        else
        {
            // Closing bracket.
            if (bracketStack.empty())
                return false;

            const auto candidateMatchingBracket = bracketStack.top();
            switch (bracket)
            {
                case '}':
                    if (candidateMatchingBracket != '{')
                        return false;
                    break;
                case ']':
                    if (candidateMatchingBracket != '[')
                        return false;
                    break;
                case ')':
                    if (candidateMatchingBracket != '(')
                        return false;
                    break;
                default:
                    assert(false);
            }
            // Matched - pop the matching opening bracket off the stack.
            bracketStack.pop();

        }
    }
    
    return bracketStack.empty();
}

int main()
{
    int n;
    cin >> n;

    for (int i = 0; i < n; i++)
    {
        string bracketString;
        cin >> bracketString;

        if (isBalanced(bracketString))
            cout << "YES";
        else
            cout << "NO";
        cout << endl;
    }
}
