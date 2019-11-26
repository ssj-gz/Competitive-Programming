// Simon St James (ssjgz) - 2019-11-26
// 
// Solution to: https://www.codechef.com/problems/NOLOGIC
//
#include <iostream>
#include <cassert>
#include <limits>

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();
    // Swallow the rest of line, plus carriage return.
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    for (int t = 0; t < T; t++)
    {
        string question;
        getline(cin, question);
        const int numLetters = 26;
        bool isLetterUsedInQuestion[numLetters] = {};

        for (const auto questionLetter : question)
        {
            if (questionLetter >= 'a' && questionLetter <= 'z')
                isLetterUsedInQuestion[questionLetter - 'a'] = true;
            if (questionLetter >= 'A' && questionLetter <= 'Z')
                isLetterUsedInQuestion[questionLetter - 'A'] = true;
        }

        char answer = '~';

        for (int letterIndex = 0; letterIndex < numLetters; letterIndex++)
        {
            if (!isLetterUsedInQuestion[letterIndex])
            {
                answer = 'a' + letterIndex;
                break;
            }
        }

        cout << answer << '\n';
    }

    assert(cin);
}
