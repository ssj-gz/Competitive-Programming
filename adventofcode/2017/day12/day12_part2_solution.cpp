#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <sstream>

#include <cassert>

using namespace std;

int main()
{
    string commInfoLine;
    struct Program
    {
        int id = -1;
        vector<Program*> commPrograms;
        vector<int> commProgramIds;
    };
    map<int, Program> programById;
    while (getline(cin, commInfoLine))
    {
        for (auto& character : commInfoLine) if (character == ',') character = ' ';

        istringstream commInfoStream(commInfoLine);
        Program program;
        commInfoStream >> program.id;
        string dummy;
        commInfoStream >> dummy;
        assert(dummy == "<->");
        int commProgramId = -1;
        while (commInfoStream >> commProgramId)
        {
            program.commProgramIds.push_back(commProgramId);
        }
        programById[program.id] = program;
    }

    for (auto& [id, program] : programById)
    {
        for (auto commProgramId : program.commProgramIds)
        {
            assert(programById.contains(commProgramId));
            program.commPrograms.push_back(&(programById[commProgramId]));
        }
    }

    vector<const Program*> allProgs;
    for (auto& [id, program] : programById)
        allProgs.push_back(&program);

    set<const Program*> seen;
    int numGroups = 0;
    for (const auto* startingProgram : allProgs)
    {
        if (seen.contains(startingProgram))
            continue;
        std::cout << "Beginning group" << std::endl;
        std::cout << " " << startingProgram->id << std::endl;
        vector<const Program*> toExplore = { startingProgram };
        seen.insert(startingProgram);
        numGroups++;
        while (!toExplore.empty())
        {
            vector<const Program*> nextToExplore;
            for (const auto* program : toExplore)
            {
                for (const auto* neighbourProgram : program->commPrograms)
                {
                    if(!seen.contains(neighbourProgram))
                    {
                        nextToExplore.push_back(neighbourProgram);
                        seen.insert(neighbourProgram);
                        std::cout << " " << neighbourProgram->id << std::endl;
                    }
                }
            }
            toExplore = nextToExplore;
        }
    }
    std::cout << "numGroups: " << numGroups << std::endl;
    

    return 0;
}
