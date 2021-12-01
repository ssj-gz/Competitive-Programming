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
        int minOccurences = -1;
        lineStream >> minOccurences;
        char dummyChar;
        lineStream >> dummyChar;
        assert(dummyChar == '-');
        int maxOccurences = -1;
        lineStream >> maxOccurences;

        char letterToCount;
        lineStream >> letterToCount;
        lineStream >> dummyChar;
        assert(dummyChar == ':');

        string passwordToCheck;
        lineStream >> passwordToCheck;

        assert(lineStream);

        const int numOccurencesOfLetter = static_cast<int>(count(passwordToCheck.begin(), passwordToCheck.end(), letterToCount));
        if ((minOccurences <= numOccurencesOfLetter) && (numOccurencesOfLetter <= maxOccurences))
            validPasswordCount++;

    }
    cout << validPasswordCount << endl;
}
