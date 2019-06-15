#include <iostream>
#include <string>

using namespace std;

int main()
{
    // Easy one - two words A and B are anagrams if and only if
    // A has the same number of 'a's as B; the same number of 'b's;
    // etc for each letter of the alphabet i.e. they have the same
    // "letter histogram".
    //
    // If they have differing numbers of some letter, then we need to 
    // remove as many occurrences of that letter from the string
    // which has the surplus of that letter as are needed to obtain equality.
    // This number is basically abs(letterHistogramA[letter] - letterHistogramB[letter]).
    string a;
    cin >> a;

    string b;
    cin >> b;

    const auto numLetters = 26;
    int letterHistogramA[numLetters] = {};
    int letterHistogramB[numLetters] = {};

    for (const auto letter : a)
    {
        letterHistogramA[letter - 'a']++;
    }
    for (const auto letter : b)
    {
        letterHistogramB[letter - 'a']++;
    }

    int numToDelete = 0;
    for (int i = 0; i < numLetters; i++)
    {
        numToDelete += abs(letterHistogramA[i] - letterHistogramB[i]);
    }

    cout << numToDelete << endl;
}
