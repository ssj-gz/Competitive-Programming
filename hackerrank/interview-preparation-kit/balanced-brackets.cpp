// Simon St James (ssjg) - 2019-04-05
#include <iostream>
#include <stack>

#include <cassert>

using namespace std;

bool isBalanced(const string& bracketString)
{
    std::stack<char> openingBracketStack;

    for (const auto bracket : bracketString)
    {
        if (bracket == '{' || bracket == '[' || bracket == '(')
        {
            // Opening bracket.
            openingBracketStack.push(bracket);
        }
        else
        {
            // Closing bracket.
            if (openingBracketStack.empty())
                return false;

            const auto candidateMatchingBracket = openingBracketStack.top();
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
            openingBracketStack.pop();

        }
    }
    
    // To be balanced, there must be no opening brackets remaining on the stack.
    return openingBracketStack.empty();
}

int main()
{
    // Trivially easy (though I missed an important case initially XD); hopefully the code is self-explanatory :)
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
