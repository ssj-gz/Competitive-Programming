#include <iostream>
#include <vector>
#include <unordered_map>
#include <cassert>

using namespace std;

int main()
{
    int64_t startingNumber;
    int64_t lastSpokenNumber = -1;
    unordered_map<int64_t, vector<int>> turnsSpokenOn;
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
    while (numSpokenNumbers < 30000000)
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

        //cout << "next number: " << number << endl;
        if (turnIndex % 10'000 == 0)
            cout << "turn: " << turnIndex << endl;

        lastSpokenNumber = number;
        turnsSpokenOn[number].push_back(turnIndex);
        turnIndex++;
        numSpokenNumbers++;
    }
    cout << lastSpokenNumber << endl;
}
