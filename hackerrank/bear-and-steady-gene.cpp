#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cassert>
using namespace std;


int main() {
    long n;
    cin >> n;
    string s;
    cin >> s;
    // Convert string to string of integers 0..3 - A = 0, C = 1, G = 2, T = 3 - makes
    // life a little easier :)
    for (auto& character : s)
    {
        switch(character)
        {
            case 'A':
                character = 0;
                break;
            case 'C':
                character = 1;
                break;
            case 'G':
                character = 2;
                break;
            case 'T':
                character = 3;
                break;
            default:
                assert(false && "Unrecognised char!");
        }
    }
    const int numLetters = 4;
    long letterCount[numLetters] = {};
    for (auto& letterIndex : s)
    {
        letterCount[letterIndex]++;
    }
    const long letterCountIfStable = n / numLetters;
    bool hasSurplusOfLetter[numLetters] = {};

    // Basic principle: find the "surplus" for each letter e.g. if n = 20, the letterCountIfStable is
    // 5, so if there are 7 T's and 6 A's, T has a surplus of 7 - 5 = 2, A, 6 - 5 = 1.
    // A sequence can be swapped out with another that stabilises the gene if and only if
    // that sequence contains the surplus amount of each letter i.e. in this example,
    // a stabilising sequence would need to contain 2 T's and 1 A.
    // We need to find the smallest such sequence: this has a simple "sliding range",
    // "accumulate at the front; throw off at the back" implementation.
    bool isStable = true;
    long numRequiredInSequence[numLetters] = {};
    for (int letterIndex = 0; letterIndex < numLetters; letterIndex++)
    {
        const long letterSurplus = letterCount[letterIndex] - letterCountIfStable;
        if (letterSurplus > 0)
        {
            hasSurplusOfLetter[letterIndex] =  true;
            isStable = false;
            numRequiredInSequence[letterIndex] = letterSurplus;
        }
    }
    if (isStable)
    {
        cout << "0" << endl;
        return 0;
    }
    long smallestStabilisingSequenceLength = numeric_limits<long>::max();
    string::size_type rightPointer = 0;
    string::size_type leftPointer = 0;
    while (rightPointer < s.size())
    {
        const int letterIndex = s[rightPointer];
        if (hasSurplusOfLetter[letterIndex])
        {
            numRequiredInSequence[letterIndex]--;
            bool sequenceCanStabiliseGene = true;
            for (auto numRequiredLetter : numRequiredInSequence)
            {
                if (numRequiredLetter > 0)
                    sequenceCanStabiliseGene = false;
            }
            if (sequenceCanStabiliseGene)
            {
                // Start throwing out letters from the beginning of the sequence that are no longer
                // required for this sequence to be stabilising.
                int leftmostLetterIndex = s[leftPointer];
                while (!hasSurplusOfLetter[leftmostLetterIndex] || numRequiredInSequence[leftmostLetterIndex] < 0)
                {
                    if (hasSurplusOfLetter[leftmostLetterIndex])
                        numRequiredInSequence[leftmostLetterIndex]++;

                    leftPointer++;
                    leftmostLetterIndex = s[leftPointer];
                }
                assert(rightPointer >= leftPointer);
                const long sequenceLength = rightPointer - leftPointer + 1;
                smallestStabilisingSequenceLength = min(smallestStabilisingSequenceLength, sequenceLength);
            }
        }
        rightPointer++;
    }
    cout << smallestStabilisingSequenceLength << endl;

    return 0;
}

