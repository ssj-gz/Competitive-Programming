#include <iostream>
#include <stack>
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;

int64_t getScore(const string& navSystem)
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
                    return 0;
                break;
            case ')':
                pop = true;
                if (openBraceTypes.empty() || openBraceTypes.top() != '(')
                    return 0;
                break;
            case ']':
                pop = true;
                if (openBraceTypes.empty() || openBraceTypes.top() != '[')
                    return 0;
                break;
            case '}':
                pop = true;
                if (openBraceTypes.empty() || openBraceTypes.top() != '{')
                    return 0;
                break;
        }
        if (pop)
            openBraceTypes.pop();
    }
    int64_t score = 0;
    while (!openBraceTypes.empty())
    {
        score *= 5;
        switch (openBraceTypes.top())
        {
            case '(':
                score += 1;
                break;
            case '[':
                score += 2;
                break;
            case '{':
                score += 3;
                break;
            case '<':
                score += 4;
                break;
        }
        openBraceTypes.pop();
    }
    return score;
}

int main()
{
    string navSystem;
    vector<int64_t> scores;
    while (cin >> navSystem)
    {
        const auto score =  getScore(navSystem);
        if (score != 0)
            scores.push_back( getScore(navSystem));
    }
    sort(scores.begin(), scores.end());
    cout << "Scores: " << endl;
    for (const auto score : scores)
    {
        cout << score << endl;
    }
    assert(scores.size() % 2 == 1);
    cout << scores[scores.size() / 2] << endl;

}
