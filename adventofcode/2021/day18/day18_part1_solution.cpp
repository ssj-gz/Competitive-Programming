#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;

struct SnailFishNumber
{
    enum { RegularNumber, Pair } type = RegularNumber;

    int64_t value = -1;

    std::pair<SnailFishNumber*, SnailFishNumber*> pair = {nullptr, nullptr};
    SnailFishNumber *parent = nullptr;
};

SnailFishNumber* parse(const string& snailString)
{
    cout << "parsing: " << snailString << endl;
    SnailFishNumber* snailFishNumber = new SnailFishNumber();
    if (snailString.front() == '[')
    {
        snailFishNumber->type = SnailFishNumber::Pair;

        assert(snailString.back() == ']');
        int nestingDepth = 1;
        int topLevelCommaPos = -1;
        for (int pos = 1; pos < snailString.size() - 1; pos++)
        {
            const char chr = snailString[pos];
            if (chr == '[')
                nestingDepth++;
            else if (chr == ']')
                nestingDepth--;
            else if (chr == ',' && nestingDepth == 1)
            {
                assert(topLevelCommaPos == -1);
                topLevelCommaPos = pos;
            }
        }
        assert(topLevelCommaPos != -1);
        assert(nestingDepth == 1);

        snailFishNumber->pair.first = parse(snailString.substr(1, topLevelCommaPos - 1));
        snailFishNumber->pair.second = parse(snailString.substr(topLevelCommaPos + 1, snailString.size() - 1 - topLevelCommaPos - 1));

        snailFishNumber->pair.first->parent = snailFishNumber;
        snailFishNumber->pair.second->parent = snailFishNumber;
    }
    else
    {
        snailFishNumber->value = stol(snailString);
    }
    return snailFishNumber;
}

void listAllRegularNumbersInOrder(SnailFishNumber* root, vector<SnailFishNumber*>& destParts)
{
    if (root->type == SnailFishNumber::Pair)
    {
        listAllRegularNumbersInOrder(root->pair.first, destParts);
        listAllRegularNumbersInOrder(root->pair.second, destParts);
    }
    else
    {
        destParts.push_back(root);
    }
}

SnailFishNumber* findRegularNumberToLeft(SnailFishNumber* snailFishNumber)
{
    SnailFishNumber* topLevel = snailFishNumber;
    while (topLevel->parent != nullptr)
        topLevel = topLevel->parent;

    vector<SnailFishNumber*> allRegularNumbersInOrder;
    listAllRegularNumbersInOrder(topLevel, allRegularNumbersInOrder);

    cout << "allRegularNumbersInOrder (numbers): " << endl;
    for (auto* part : allRegularNumbersInOrder)
    {
        assert(part->type == SnailFishNumber::RegularNumber);
        cout << part->value << " ";
    }
    cout << endl;

    SnailFishNumber *result = nullptr;
    for (auto* regularNumber : allRegularNumbersInOrder)
    {
        if (regularNumber == snailFishNumber)
            return result;
        result = regularNumber;
    }
    return nullptr;

}

SnailFishNumber* findRegularNumberToRight(SnailFishNumber* snailFishNumber)
{
    SnailFishNumber* topLevel = snailFishNumber;
    while (topLevel->parent != nullptr)
        topLevel = topLevel->parent;

    vector<SnailFishNumber*> allRegularNumbersInOrder;
    listAllRegularNumbersInOrder(topLevel, allRegularNumbersInOrder);

    reverse(allRegularNumbersInOrder.begin(), allRegularNumbersInOrder.end());

    SnailFishNumber *result = nullptr;
    for (auto* regularNumber : allRegularNumbersInOrder)
    {
        if (regularNumber == snailFishNumber)
            return result;
        result = regularNumber;
    }
    return nullptr;
}

void printFlat(SnailFishNumber* snailFishNumber)
{
    if (snailFishNumber->type == SnailFishNumber::Pair)
    {
        cout << "[";
        printFlat(snailFishNumber->pair.first);
        cout << ",";
        printFlat(snailFishNumber->pair.second);
        cout << "]";

    }
    else
    {
        cout << snailFishNumber->value;
    }
}

bool doExplode(SnailFishNumber* snailFishNumber, int nestingDepth = 0)
{
    assert(nestingDepth <= 4);
    if (nestingDepth == 4 && snailFishNumber->type == SnailFishNumber::Pair)
    {
        cout << "Exploding: ";
        printFlat(snailFishNumber);
        cout << endl;
        assert(snailFishNumber->pair.first->type == SnailFishNumber::RegularNumber);
        assert(snailFishNumber->pair.second->type == SnailFishNumber::RegularNumber);
        SnailFishNumber* regularNumberToLeft = findRegularNumberToLeft(snailFishNumber->pair.first);
        SnailFishNumber* regularNumberToRight = findRegularNumberToRight(snailFishNumber->pair.second);
        cout << "regularNumberToLeft: " << (regularNumberToLeft ? to_string(regularNumberToLeft->value) : "none") << endl;
        cout << "regularNumberToRight: " << (regularNumberToRight ? to_string(regularNumberToRight->value) : "none") << endl;
        assert(regularNumberToLeft != snailFishNumber);
        assert(regularNumberToRight != snailFishNumber);
        if (regularNumberToLeft)
        {
            regularNumberToLeft->value += snailFishNumber->pair.first->value;
        }
        if (regularNumberToRight)
        {
            cout << "Adding " << snailFishNumber->pair.second->value << " to " << regularNumberToRight->value << endl;
            regularNumberToRight->value += snailFishNumber->pair.second->value;
        }
        delete snailFishNumber->pair.first;
        delete snailFishNumber->pair.second;
        snailFishNumber->type = SnailFishNumber::RegularNumber;
        snailFishNumber->value = 0;

        return true;
    }
    if (snailFishNumber->type == SnailFishNumber::Pair)
    {
        if (doExplode(snailFishNumber->pair.first, nestingDepth + 1))
            return true;
        if (doExplode(snailFishNumber->pair.second, nestingDepth + 1))
            return true;
    }

    return false;
}

bool doSplit(SnailFishNumber* snailFishNumber)
{
    if (snailFishNumber->type == SnailFishNumber::RegularNumber)
    {
        if (snailFishNumber->value >= 10)
        {
            snailFishNumber->type = SnailFishNumber::Pair;
            const int originalValue = snailFishNumber->value;
            snailFishNumber->value = -1;
            snailFishNumber->pair.first = new SnailFishNumber();
            snailFishNumber->pair.first->value = originalValue / 2;
            snailFishNumber->pair.first->parent = snailFishNumber;
            snailFishNumber->pair.second = new SnailFishNumber();
            snailFishNumber->pair.second->value = (originalValue / 2) + ((originalValue % 2 == 1) ? 1 : 0);
            snailFishNumber->pair.second->parent = snailFishNumber;
            return true;
        }
    }
    else if (snailFishNumber->type == SnailFishNumber::Pair)
    {
        if (doSplit(snailFishNumber->pair.first))
            return true;
        if (doSplit(snailFishNumber->pair.second))
            return true;
    }

    return false;
}


void print(SnailFishNumber* snailFishNumber, string indent = "")
{
    if (snailFishNumber->type == SnailFishNumber::Pair)
    {
        cout << indent << "Pair: " << endl;
        print(snailFishNumber->pair.first, indent + " ");
        print(snailFishNumber->pair.second, indent + " ");

    }
    else
    {
        cout << indent << "Regular Number:  "  << snailFishNumber->value << endl;
    }
}

SnailFishNumber* add(SnailFishNumber* left, SnailFishNumber* right)
{
    SnailFishNumber* result = new SnailFishNumber;
    result->type = SnailFishNumber::Pair;
    result->pair.first = left;
    result->pair.second = right;
    left->parent = result;
    right->parent = result;
    cout << "After initial add: " << endl;
    print(result);
    // Reduce.
    bool changeMade = false;
    do
    {
        cout << " beginning iteration: " << endl;
        printFlat(result);
        cout << endl;
        changeMade = false;
        if (doExplode(result))
        {
            changeMade = true;
            cout << "Exploded" << endl;
            continue;
        }
        if (doSplit(result))
        {
            changeMade = true;
            cout << "Split" << endl;
            continue;
        }
    } while(changeMade);
    

    return result;
}

int64_t calcMagnitude(SnailFishNumber* root)
{
    if (root->type == SnailFishNumber::RegularNumber)
    {
        return root->value;
    }
    else
    {
        return 3 * calcMagnitude(root->pair.first) + 2 * calcMagnitude(root->pair.second);
    }
}

int main()
{
    string snailLine;
    vector<SnailFishNumber*> snailFishNumbers;
    while (getline(cin, snailLine))
    {
        snailFishNumbers.push_back(parse(snailLine));
        cout << "parsed " << snailLine << ":" << endl;
        print(snailFishNumbers.back());
    }

    SnailFishNumber *currentSnailFishNum = snailFishNumbers.front();
    for (int i = 1; i < snailFishNumbers.size(); i++)
    {
        currentSnailFishNum = add(currentSnailFishNum, snailFishNumbers[i]);
        cout << "After adding: " << endl;
        printFlat(currentSnailFishNum);
        cout << endl;
    }
    cout << "magnitude:" << calcMagnitude(currentSnailFishNum) << endl;
}
