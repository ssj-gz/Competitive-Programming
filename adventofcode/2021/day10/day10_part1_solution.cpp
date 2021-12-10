#include <iostream>
#include <stack>

using namespace std;

int getSyntaxError(const string& navSystem)
{
    stack<char> openBraceTypes;
    for (const auto x : navSystem)
    {
        bool pop = false;
        switch(x)
        {
            case '<':
            case '(':
            case '[':
            case '{':
                openBraceTypes.push(x);
                break;

            case '>':
                pop = true;
                if (openBraceTypes.empty() || openBraceTypes.top() != '<')
                    return 25137;
                break;
            case ')':
                pop = true;
                if (openBraceTypes.empty() || openBraceTypes.top() != '(')
                    return 3;
                break;
            case ']':
                pop = true;
                if (openBraceTypes.empty() || openBraceTypes.top() != '[')
                    return 57;
                break;
            case '}':
                pop = true;
                if (openBraceTypes.empty() || openBraceTypes.top() != '{')
                    return 1197;
                break;
        }
        if (pop)
            openBraceTypes.pop();
    }
    return 0;
}

int main()
{
    string navSystem;
    int64_t totalSyntaxError = 0;
    while (cin >> navSystem)
    {
        totalSyntaxError += getSyntaxError(navSystem);
    }
    cout << totalSyntaxError << endl;

}
