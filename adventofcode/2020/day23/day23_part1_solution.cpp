#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main()
{
    string input;
    cin >> input;

    const int numCups = static_cast<int>(input.size());

    vector<int> cupValues;
    for (const auto inputDigit : input)
        cupValues.push_back(inputDigit - '0');


    int currentCup = cupValues.front();
    int moveNum = 0;
    while (true)
    {
        moveNum++;
        cout << "-- move " << moveNum << " --" << endl;

        auto printCups = [&cupValues, &currentCup]()
        {
            cout << "cups: ";
            for (const auto cup : cupValues)
            {
                if (cup == currentCup)
                    cout << "(" << cup << ")";
                else
                    cout << cup;
                cout << " ";
            }
            cout << endl;
        };

        printCups();

        vector<int> threeCups;
        int cupToRemoveIndex = static_cast<int>(std::find(cupValues.begin(), cupValues.end(), currentCup) - cupValues.begin()) + 1;
        for (int i = 0; i < 3; i++)
        {
            if (cupToRemoveIndex >= static_cast<int>(cupValues.size()))
                cupToRemoveIndex = 0;
            threeCups.push_back(cupValues[cupToRemoveIndex]);
            cupValues.erase(cupValues.begin() + cupToRemoveIndex);
        }
        cout << "pick up:" << threeCups[0] << "," << threeCups[1] << "," << threeCups[2] << endl;
        int destination = currentCup - 1;
        while (true)
        {
            if (destination == 0)
                destination = numCups;
            if (find(threeCups.begin(), threeCups.end(), destination) == threeCups.end())
                break;
            destination--;
        }
        cout << "destination: " << destination << endl << endl;
        copy(threeCups.begin(), threeCups.end(), inserter(cupValues, std::next(std::find(cupValues.begin(), cupValues.end(), destination))));

        auto nextCupIter = std::next(find(cupValues.begin(), cupValues.end(), currentCup));
        if (nextCupIter == cupValues.end())
            nextCupIter = cupValues.begin();

        currentCup = *nextCupIter;

        if (moveNum == 100)
        {
            cout << "-- final --" <<endl;
            printCups();
            break;
        }
    }

    const auto cup1Iter = std::find(cupValues.begin(), cupValues.end(), 1);
    std::rotate(cupValues.begin(), cup1Iter, cupValues.end());
    cupValues.erase(cupValues.begin());
    for (const auto cup : cupValues)
    {
        cout << cup;
    }
    cout << endl;

}
