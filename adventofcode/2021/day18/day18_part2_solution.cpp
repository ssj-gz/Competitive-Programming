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

    SnailFishNumber* pair[2] = {nullptr, nullptr};
    SnailFishNumber *parent = nullptr;

    ~SnailFishNumber()
    {
        if (type == Pair)
        {
            delete pair[0];
            delete pair[1];
        }
    }
};

SnailFishNumber* parse(const string& snailString)
{
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

        snailFishNumber->pair[0] = parse(snailString.substr(1, topLevelCommaPos - 1));
        snailFishNumber->pair[1] = parse(snailString.substr(topLevelCommaPos + 1, snailString.size() - 1 - topLevelCommaPos - 1));

        snailFishNumber->pair[0]->parent = snailFishNumber;
        snailFishNumber->pair[1]->parent = snailFishNumber;
    }
    else
    {
        snailFishNumber->value = stol(snailString);
    }
    return snailFishNumber;
}

void listAllRegularNumbersLeftToRight(SnailFishNumber* root, vector<SnailFishNumber*>& destParts)
{
    if (root->type == SnailFishNumber::Pair)
    {
        listAllRegularNumbersLeftToRight(root->pair[0], destParts);
        listAllRegularNumbersLeftToRight(root->pair[1], destParts);
    }
    else
    {
        destParts.push_back(root);
    }
}

vector<SnailFishNumber*> findAllRegularNumbersLeftToRightInNumContaining(SnailFishNumber* containedSnailFishNumber)
{
    SnailFishNumber* topLevel = containedSnailFishNumber;
    while (topLevel->parent != nullptr)
        topLevel = topLevel->parent;

    vector<SnailFishNumber*> allRegularNumbersLeftToRight;
    listAllRegularNumbersLeftToRight(topLevel, allRegularNumbersLeftToRight);

    return allRegularNumbersLeftToRight;
}

SnailFishNumber* findLastBefore(SnailFishNumber* before, const vector<SnailFishNumber*>& list)
{
    SnailFishNumber *result = nullptr;
    for (auto* regularNumber : list)
    {
        if (regularNumber == before)
            return result;
        result = regularNumber;
    }
    return nullptr;
}

SnailFishNumber* findRegularNumberToLeft(SnailFishNumber* snailFishNumber)
{
    const auto allRegularNumbersLeftToRight = findAllRegularNumbersLeftToRightInNumContaining(snailFishNumber);
    return findLastBefore(snailFishNumber, allRegularNumbersLeftToRight);
}

SnailFishNumber* findRegularNumberToRight(SnailFishNumber* snailFishNumber)
{

    auto allRegularNumbersLeftToRight = findAllRegularNumbersLeftToRightInNumContaining(snailFishNumber);
    reverse(allRegularNumbersLeftToRight.begin(), allRegularNumbersLeftToRight.end());
    return findLastBefore(snailFishNumber, allRegularNumbersLeftToRight);
}

void printFlat(SnailFishNumber* snailFishNumber)
{
    if (snailFishNumber->type == SnailFishNumber::Pair)
    {
        cout << "[";
        printFlat(snailFishNumber->pair[0]);
        cout << ",";
        printFlat(snailFishNumber->pair[1]);
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
        assert(snailFishNumber->pair[0]->type == SnailFishNumber::RegularNumber);
        assert(snailFishNumber->pair[1]->type == SnailFishNumber::RegularNumber);
        SnailFishNumber* regularNumberToLeft = findRegularNumberToLeft(snailFishNumber->pair[0]);
        SnailFishNumber* regularNumberToRight = findRegularNumberToRight(snailFishNumber->pair[1]);
        assert(regularNumberToLeft != snailFishNumber);
        assert(regularNumberToRight != snailFishNumber);
        if (regularNumberToLeft)
        {
            regularNumberToLeft->value += snailFishNumber->pair[0]->value;
        }
        if (regularNumberToRight)
        {
            regularNumberToRight->value += snailFishNumber->pair[1]->value;
        }
        delete snailFishNumber->pair[0];
        snailFishNumber->pair[0] = nullptr;
        delete snailFishNumber->pair[1];
        snailFishNumber->pair[1] = nullptr;
        snailFishNumber->type = SnailFishNumber::RegularNumber;
        snailFishNumber->value = 0;

        return true;
    }
    if (snailFishNumber->type == SnailFishNumber::Pair)
    {
        for (const auto pairElement : snailFishNumber->pair)
        {
            if (doExplode(pairElement, nestingDepth + 1))
                return true;
        }
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
            snailFishNumber->pair[0] = new SnailFishNumber();
            snailFishNumber->pair[0]->value = originalValue / 2;
            snailFishNumber->pair[0]->parent = snailFishNumber;
            snailFishNumber->pair[1] = new SnailFishNumber();
            snailFishNumber->pair[1]->value = originalValue / 2 + ((originalValue % 2 == 1) ? 1 : 0);
            snailFishNumber->pair[1]->parent = snailFishNumber;
            return true;
        }
    }
    else if (snailFishNumber->type == SnailFishNumber::Pair)
    {
        for (const auto pairElement : snailFishNumber->pair)
        {
            if (doSplit(pairElement))
                return true;
        }
    }

    return false;
}


SnailFishNumber* add(SnailFishNumber* left, SnailFishNumber* right)
{
    SnailFishNumber* result = new SnailFishNumber;
    result->type = SnailFishNumber::Pair;
    result->pair[0] = left;
    result->pair[1] = right;
    left->parent = result;
    right->parent = result;

    cout << "after addition: ";
    printFlat(result);
    cout << endl;

    // Reduce.
    bool changeMade = false;
    do
    {
        changeMade = false;
        if (doExplode(result))
        {
            changeMade = true;
            cout << "after explode:  ";
            printFlat(result);
            cout << endl;
            continue;
        }
        if (doSplit(result))
        {
            changeMade = true;
            cout << "after split  :  ";
            printFlat(result);
            cout << endl;

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
        return 3 * calcMagnitude(root->pair[0]) + 2 * calcMagnitude(root->pair[1]);
    }
}

int main()
{
    string snailLine;
    vector<string> snailFishLines;
    while (getline(cin, snailLine))
    {
        snailFishLines.push_back(snailLine);
    }
    const int numSnailFish = static_cast<int>(snailFishLines.size());

    int64_t largestMagnitudeOfSum = 0;
    for (int leftIndex = 0; leftIndex < numSnailFish; leftIndex++)
    {
        for (int rightIndex = 0; rightIndex < numSnailFish; rightIndex++)
        {
            if (leftIndex == rightIndex)
                continue;
            auto* leftSnailFishNum = parse(snailFishLines[leftIndex]);
            auto* rightSnailFishNum = parse(snailFishLines[rightIndex]);
            auto* sum = add(leftSnailFishNum, rightSnailFishNum);
            largestMagnitudeOfSum = max(largestMagnitudeOfSum, calcMagnitude(sum));
            delete sum;
        }
    }
    cout << "largestMagnitudeOfSum:" << largestMagnitudeOfSum << endl;
}
