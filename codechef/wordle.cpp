// Simon St James (ssjgz) - 2022-03-31
// 
// Solution to: https://www.codechef.com/MARCH221D/status/WORDLE
//
#include <iostream>
#include <vector>

#include <cassert>

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

int main()
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const auto hiddenWord = read<string>();
        const auto guessWord = read<string>();
        assert(hiddenWord.size() == guessWord.size());

        string correctnessString(hiddenWord.size(), ' ');
        for (string::size_type i = 0; i < correctnessString.size(); i++)
        {
            if (guessWord[i] == hiddenWord[i])
                correctnessString[i] = 'G';
            else
                correctnessString[i] = 'B';
        }
        cout << correctnessString << endl; 
    }

    assert(cin);
}
