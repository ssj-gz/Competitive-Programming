#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <cassert>

using namespace std;

struct Quantity
{
    string chemicalName;
    int amount = -1;
};

struct Formula
{
    vector<Quantity> inputs;
    Quantity output;
};

ostream& operator<<(ostream& os, const Formula& formula)
{
    os << "Formula: ";
    for (const auto& quantity : formula.inputs)
    {
        os << quantity.amount << " " << quantity.chemicalName << " ";
    }
    os << "=> ";
    os << formula.output.amount << " " << formula.output.chemicalName;

    return os;
}

struct State
{
    map<string, int> amountOfChemical;
    auto operator<=>(const State&) const = default;
};

ostream& operator<<(ostream& os, const State& state)
{
    os << "state: [";
    for (const auto& [chemicalName, amount] : state.amountOfChemical)
    {
        os << amount << " " << chemicalName << " ";
    }
    os << "]";

    return os;
}

int64_t minOreToCreateChemical(const string& chemicalName, int64_t amount, const map<string, Formula>& formulaToCreateChemical)
{
    if (chemicalName == "ORE")
        return amount;
    const auto formulaIter = formulaToCreateChemical.find(chemicalName);
    assert(formulaIter != formulaToCreateChemical.end());
    const auto& formula = formulaIter->second;

    int64_t minOre = 0;
    int64_t numTimesToApplyFormula = (amount / formula.output.amount);
    while (numTimesToApplyFormula * formula.output.amount < amount)
    {
        numTimesToApplyFormula++;
    }
    for (const auto& quantity : formula.inputs)
    {
        minOre += minOreToCreateChemical(quantity.chemicalName, numTimesToApplyFormula * quantity.amount, formulaToCreateChemical);
    }
    return minOre;
}

int64_t minOreToCreateFuel(map<string, Formula>& formulaToCreateChemical)
{
    map<string, int64_t> amountOfChemicalNeeded {{ "FUEL", 1}};
    while (true)
    {
        bool needNonOre = false;
        for (const auto& [chemicalName, amountNeeded] : amountOfChemicalNeeded)
        {
            if (chemicalName == "ORE")
                continue;
            if (amountNeeded > 0)
            {
                cout << "Need some " << chemicalName << "(" << amountNeeded << ")" << endl;
                needNonOre = true;
                const auto formulaIter = formulaToCreateChemical.find(chemicalName);
                assert(formulaIter != formulaToCreateChemical.end());
                const auto& formula = formulaIter->second;

                int64_t numTimesToApplyFormula = (amountNeeded / formula.output.amount);
                while (numTimesToApplyFormula * formula.output.amount < amountNeeded)
                {
                    numTimesToApplyFormula++;
                }
                for (const auto& quantity : formula.inputs)
                {
                    amountOfChemicalNeeded[quantity.chemicalName] += numTimesToApplyFormula * quantity.amount;
                }
                amountOfChemicalNeeded[chemicalName] -= numTimesToApplyFormula * formula.output.amount;
                assert(amountOfChemicalNeeded[chemicalName] <= 0);
                break;
            }
        }
        if (!needNonOre)
            return amountOfChemicalNeeded["ORE"];
    }
    assert(false && "Unreachable");
    return -1;
}

int main()
{

    string formulaLine;
    vector<Formula> formulae;
    while (getline(cin, formulaLine))
    {
        cout << "formulaLine: " << formulaLine << endl;
        Formula formula;

        auto posOfInputsEnd = formulaLine.find("=>");
        assert(posOfInputsEnd != string::npos);

        auto inputsString = formulaLine.substr(0, posOfInputsEnd);
        for (auto& character : inputsString)
            if (character == ',')
                character = ' ';

        istringstream inputsStream(inputsString);

        while (true)
        {
            int amount = -1;
            inputsStream >> amount;
            string chemicalName;
            inputsStream >> chemicalName;


            if (!inputsStream)
                break;

            formula.inputs.push_back({chemicalName, amount});
        }

        auto outputString = formulaLine.substr(posOfInputsEnd + 2);
        istringstream outputsStream(outputString);

        int amount = -1;
        outputsStream >> amount;
        string chemicalName;
        outputsStream >> chemicalName;
        assert(outputsStream);

        formula.output = {chemicalName, amount};
        cout << formula << endl;

        formulae.push_back(formula);
    }

    map<string, int> numWaysToCreateChemical;
    map<string, Formula> formulaToCreateChemical;
    for (const auto& formula :formulae)
    {
        numWaysToCreateChemical[formula.output.chemicalName]++;
        assert(!formulaToCreateChemical.contains(formula.output.chemicalName) && "Can't deal with multiple paths that create the same chemical");
        formulaToCreateChemical[formula.output.chemicalName] = formula;
    }

    cout << "blah: " << minOreToCreateFuel(formulaToCreateChemical) << endl;
}
