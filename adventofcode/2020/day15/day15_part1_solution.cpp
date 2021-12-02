#include <iostream>
#include <vector>
#include <map>
#include <cassert>

using namespace std;

int main()
{
    int64_t startingNumber;
    int64_t lastSpokenNumber = -1;
    map<int64_t, vector<int>> turnsSpokenOn;
    int turnIndex = 0;
    while (cin >> startingNumber)
    {
        lastSpokenNumber = startingNumber;
        turnsSpokenOn[startingNumber].push_back(turnIndex);

        char dummyChar = '\0';
        cin >> dummyChar;
        assert(!cin || dummyChar == ',');

        turnIndex++;
    }

    int numSpokenNumbers = turnIndex;
    while (numSpokenNumbers < 2020)
    {
        int64_t number = 0;
        const auto& turnsLastSpokenNumberSpokenOn = turnsSpokenOn[lastSpokenNumber];
        if (turnsLastSpokenNumberSpokenOn.size() >= 2)
        {
            auto blah = std::prev(turnsLastSpokenNumberSpokenOn.end());
            number = *blah;
            blah--;
            number -= *blah;
        }

        cout << "next number: " << number << endl;

        lastSpokenNumber = number;
        turnsSpokenOn[number].push_back(turnIndex);
        turnIndex++;
        numSpokenNumbers++;
    }
    cout << lastSpokenNumber << endl;
}
