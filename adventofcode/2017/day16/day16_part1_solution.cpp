#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>

#include <cassert>

using namespace std;

int main()
{
    constexpr int numPrograms = 16;
    //constexpr int numPrograms = 5;
    vector<char> programLineup;
    for (int i = 0; i < numPrograms; i++)
        programLineup.push_back('a' + i);

    string danceInstructions;
    getline(cin, danceInstructions);
    std::replace(danceInstructions.begin(), danceInstructions.end(), ',', ' ');

    istringstream danceInstructionStream(danceInstructions);
    string instruction;
    while (danceInstructionStream >> instruction)
    {
        std::replace(instruction.begin(), instruction.end(), '/', ' ');
        istringstream instructionStream(instruction);
        char instructionType = '\0';
        instructionStream >> instructionType;
        switch (instructionType)
        {
            case 's':
                {
                    int spinAmount = -1;
                    instructionStream >> spinAmount;
                    assert(spinAmount >= 0 && spinAmount < numPrograms);
                    // TODO - use std::rotate instead?
                    const vector<char> endPrograms(programLineup.end() - spinAmount, programLineup.end());
                    programLineup.erase(programLineup.end() - spinAmount, programLineup.end());
                    programLineup.insert(programLineup.begin(), endPrograms.begin(), endPrograms.end());
                }
                break;
            case 'x':
                {
                    int posA = -1;
                    int posB = -1;
                    instructionStream >> posA;
                    instructionStream >> posB;
                    assert(posA >= 0 && posA < numPrograms);
                    assert(posB >= 0 && posB < numPrograms);
                    swap(programLineup[posA], programLineup[posB]);
                }
                break;
            case 'p':
                {
                    char programA = '\0';
                    char programB = '\0';
                    instructionStream >> programA;
                    instructionStream >> programB;
                    assert(programA >= 'a' && programA <= 'a' + numPrograms);
                    assert(programB >= 'a' && programB <= 'a' + numPrograms);
                    const auto programAIter = std::find(programLineup.begin(), programLineup.end(), programA);
                    assert(programAIter != programLineup.end());
                    const auto programBIter = std::find(programLineup.begin(), programLineup.end(), programB);
                    assert(programBIter != programLineup.end());
                    iter_swap(programAIter, programBIter);
                }
                break;
        }
    }

    std::cout << "final order: " << std::endl;
    for (const auto program : programLineup)
        std::cout << program;
    std::cout << endl;



    return 0;
}
