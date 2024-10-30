#include <iostream>
#include <regex>
#include <sstream>
#include <map>
#include <algorithm>

#include <cassert>

using namespace std;

int main()
{
    std::regex listRegex(R"(^(\w+)\s*\((\d+)\)\s*(-> (.*))?$)");
    string listLine;
    struct Program
    {
        string name;
        int weight = -1;
        vector<Program*> supportedPrograms;
        vector<string> supportedProgramNames;
    };
    map<string, Program> programForName;
    while (getline(cin, listLine))
    {
        std::smatch listMatch;
        const bool matchSuccessful = std::regex_match(listLine, listMatch, listRegex);
        assert(matchSuccessful);
        Program program;
        program.name = listMatch[1];
        std::cout << "name: " << program.name << std::endl;
        program.weight = std::stoi(listMatch[2]);
        std::string supportedProgramDesc = listMatch[4];
        supportedProgramDesc.erase(std::remove(supportedProgramDesc.begin(), supportedProgramDesc.end(), ','), supportedProgramDesc.end());
        std::cout << "supportedProgramDesc: " << supportedProgramDesc << std::endl;
        istringstream supportedProgramsStream(supportedProgramDesc);
        string supportedProgramName;
        while (supportedProgramsStream >> supportedProgramName)
            program.supportedProgramNames.push_back(supportedProgramName);

        programForName[program.name] = program;
    }
    for(auto& [programName, program] : programForName)
    {
        for (const auto& supportedProgramName : program.supportedProgramNames)
        {
            assert(programForName.contains(supportedProgramName));
            program.supportedPrograms.push_back(&programForName[supportedProgramName]);
        }
    }

    string bottomProgramName;
    for(auto& [programName, program] : programForName)
    {
        bool isSupported = false;
        for(auto& [otherProgramName, otherProgram] : programForName)
        {
            if (otherProgramName == programName)
                continue;

            if (std::find(otherProgram.supportedPrograms.begin(), otherProgram.supportedPrograms.end(), &program) != otherProgram.supportedPrograms.end())
                isSupported = true;
        }
        if (!isSupported)
        {
            assert(bottomProgramName.empty());
            bottomProgramName = programName;
        }
    }
    assert(!bottomProgramName.empty());
    std::cout << "bottomProgramName: " << bottomProgramName << std::endl;

    return 0;
}
