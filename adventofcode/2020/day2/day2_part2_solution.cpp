#include <iostream>
#include <sstream>
#include <algorithm>
#include <cassert>

using namespace std;

int main()
{
    string line;
    int validPasswordCount = 0;
    while (getline(cin, line))
    {
        istringstream lineStream(line);
        int firstPositionToCheck = -1;
        lineStream >> firstPositionToCheck;
        char dummyChar;
        lineStream >> dummyChar;
        assert(dummyChar == '-');
        int secondPositionToCheck = -1;
        lineStream >> secondPositionToCheck;

        char expectedLetter;
        lineStream >> expectedLetter;
        lineStream >> dummyChar;
        assert(dummyChar == ':');

        string passwordToCheck;
        lineStream >> passwordToCheck;

        assert(lineStream);

        const bool expectedLetterIsAtFirstPosition = (passwordToCheck[firstPositionToCheck - 1] == expectedLetter);
        const bool expectedLetterIsAtSecondPosition = (passwordToCheck[secondPositionToCheck - 1] == expectedLetter);
        //cout << "passwordToCheck: " << passwordToCheck << " expectedLetterIsAtFirstPosition: " << expectedLetterIsAtFirstPosition << " expectedLetterIsAtSecondPosition: " << expectedLetterIsAtSecondPosition << endl;

        if (expectedLetterIsAtFirstPosition ^ expectedLetterIsAtSecondPosition)
            validPasswordCount++;

    }
    cout << validPasswordCount << endl;
}
