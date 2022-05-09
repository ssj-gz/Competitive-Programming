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
    int64_t amount = -1;
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
    map<string, int64_t> amountOfChemical;
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

int64_t minOreToCreateFuel(int64_t amountOfFuel, const map<string, Formula>& formulaToCreateChemical)
{
    map<string, int64_t> amountOfChemicalNeeded {{ "FUEL", amountOfFuel}};
    while (true)
    {
        bool needNonOre = false;
        for (const auto& [chemicalName, amountNeeded] : amountOfChemicalNeeded)
        {
            if (chemicalName == "ORE")
                continue;
            if (amountNeeded > 0)
            {
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
            int64_t amount = -1;
            inputsStream >> amount;
            string chemicalName;
            inputsStream >> chemicalName;


            if (!inputsStream)
                break;

            formula.inputs.push_back({chemicalName, amount});
        }

        auto outputString = formulaLine.substr(posOfInputsEnd + 2);
        istringstream outputsStream(outputString);

        int64_t amount = -1;
        outputsStream >> amount;
        string chemicalName;
        outputsStream >> chemicalName;
        assert(outputsStream);

        formula.output = {chemicalName, amount};
        cout << formula << endl;

        formulae.push_back(formula);
    }

    map<string, int64_t> numWaysToCreateChemical;
    map<string, Formula> formulaToCreateChemical;
    for (const auto& formula :formulae)
    {
        numWaysToCreateChemical[formula.output.chemicalName]++;
        assert(!formulaToCreateChemical.contains(formula.output.chemicalName) && "Can't deal with multiple paths that create the same chemical");
        formulaToCreateChemical[formula.output.chemicalName] = formula;
    }

    //cout << "blah: " << minOreToCreateChemical("FUEL", 1, formulaToCreateChemical) << endl;
    constexpr int64_t oreAvailable = 1'000'000'000'000LL;
    int64_t highestAmountOfFuel = 1;
    while (true)
    {
        highestAmountOfFuel *= 2;
        if (minOreToCreateFuel(highestAmountOfFuel, formulaToCreateChemical) <= oreAvailable)
        {
            cout << "Can create " << highestAmountOfFuel << endl;
        }
        else
        {
            break;
        }
    }
    cout << "highestAmountOfFuel: " << highestAmountOfFuel << endl;
    int64_t lowestAmountOfFuel = 1;
    while (highestAmountOfFuel - lowestAmountOfFuel > 1)
    {
        const int64_t midAmountOfFuel = lowestAmountOfFuel + (highestAmountOfFuel - lowestAmountOfFuel) / 2;
        if (minOreToCreateFuel(midAmountOfFuel, formulaToCreateChemical) <= oreAvailable)
        {
            cout << "Can create " << midAmountOfFuel << " of fuel" << endl;
            lowestAmountOfFuel = midAmountOfFuel;
        }
        else
        {
            cout << "Can *NOT* create " << midAmountOfFuel << " of fuel" << endl;
            highestAmountOfFuel = midAmountOfFuel - 1;
        }

    }

    while (minOreToCreateFuel(lowestAmountOfFuel, formulaToCreateChemical) <= oreAvailable)
    {
        lowestAmountOfFuel++;
    }
    lowestAmountOfFuel--;
    cout << "max fuel: " << lowestAmountOfFuel << endl;
    
#if 0
    int oreAmount = 2'000'000;
    int requiredOreAmount = -1;
    set<State> seenStates;
    while (requiredOreAmount == -1)
    {
        oreAmount++;

        State initialState;
        initialState.amountOfChemical["ORE"] = oreAmount;
        vector<State> statesToExplore = { initialState };
        cout << "Starting with " << oreAmount << " ORE" << " # seenStates: " << seenStates.size() << endl;

        while (!statesToExplore.empty())
        {
            cout << "# states to explore: " << statesToExplore.size() << endl;
            vector<State> nextStatesToExplore;
            for (const State& state : statesToExplore)
            {
                for (const auto& formulaToApply : formulae)
                {
                    State nextState = state;
                    bool appliedSuccessfully = true;
                    for (const auto& inputQuantity : formulaToApply.inputs)
                    {
                        nextState.amountOfChemical[inputQuantity.chemicalName] -= inputQuantity.amount;
                        if (nextState.amountOfChemical[inputQuantity.chemicalName] < 0)
                        {
                            appliedSuccessfully = false;
                            //cout << "failed to apply formula: " << formulaToApply << " at state: " << state << endl;
                            break;
                        }
                    }
                    if (appliedSuccessfully)
                    {
                        //cout << "appliedSuccessfully!" << endl;
                        nextState.amountOfChemical[formulaToApply.output.chemicalName] += formulaToApply.output.amount;
                        if (!seenStates.contains(nextState))
                        {
                            seenStates.insert(nextState);
                            nextStatesToExplore.push_back(nextState);
                        }
                    }
                    if (nextState.amountOfChemical["FUEL"] >= 1)
                        requiredOreAmount = oreAmount;
                }
            }
            statesToExplore = nextStatesToExplore;
        }

    }
    cout << "requiredOreAmount: " << requiredOreAmount << endl;
#endif
}
