// Simon St James (ssjgz) - 2019-10-29
// 
// Solution to: https://www.codechef.com/problems/LAPIN
//
#include <iostream>
#include <array>

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

constexpr auto numLetters = 26;

array<int, numLetters> letterHistogram(const string& s)
{
    array<int, numLetters> letterHistogram = {};
    for (const auto letter : s)
    {
        letterHistogram[letter - 'a']++;
    }
    return letterHistogram;
}

bool isLapindrome(const string& s)
{
    const int length = s.length();
    const auto leftHalf = s.substr(0, length / 2);
    const auto rightHalf = s.substr(length - length / 2);

    return letterHistogram(leftHalf) == letterHistogram(rightHalf);
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const auto s = read<string>();
        cout << (isLapindrome(s) ? "YES" : "NO") << endl;
    }

    assert(cin);
}
