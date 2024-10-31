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

    const auto* program0 = &(programById[0]);
    vector<const Program*> toExplore = { program0 };
    set<const Program*> seen = { program0 };
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
                }
            }
        }
        toExplore = nextToExplore;
    }
    std::cout << "result: " << seen.size() << std::endl;
    

    return 0;
}
