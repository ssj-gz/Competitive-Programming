#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <sstream>
#include <cassert>

using namespace std;

struct DirEntry
{
    string name;
    bool isDir = false;
    int64_t size = -1;
    auto operator<=>(const DirEntry& other) const = default;
};

void printDirTree(map<vector<string>, set<DirEntry>>& entriesForPath, const vector<string>& currentDirComponents)
{
    string indent(2 * currentDirComponents.size(), ' ');
    const auto& entriesForCurrentPath = entriesForPath[currentDirComponents];

    for (const auto& entry : entriesForCurrentPath)
    {
        cout << indent;
        cout << "- " << entry.name << " (";
        if (entry.isDir)
        {
            cout << "dir)" << endl;
            vector<string> subdirComponents(currentDirComponents);
            subdirComponents.push_back(entry.name);
            printDirTree(entriesForPath, subdirComponents);
        }
        else
        {
            cout << "file, size=" << entry.size << ")" << endl;
        }
    }
}

int64_t calcDirSize(map<vector<string>, set<DirEntry>>& entriesForPath, const vector<string>& currentDirComponents, vector<int64_t>& destDirSizes)
{

    const auto& entriesForCurrentPath = entriesForPath[currentDirComponents];

    int64_t dirSize = 0;
    for (const auto& entry : entriesForCurrentPath)
    {
        if (entry.isDir)
        {
            vector<string> subdirComponents(currentDirComponents);
            subdirComponents.push_back(entry.name);
            dirSize += calcDirSize(entriesForPath, subdirComponents, destDirSizes);
        }
        else
        {
            dirSize += entry.size;
        }
    }

    std::cout << "size of  dir: ";
    for (const auto& component : currentDirComponents)
    {
        cout << component << "/";
    }
    std::cout << " == " << dirSize << std::endl;

    destDirSizes.push_back(dirSize);

    return dirSize;
}

int main()
{
    string line;
    vector<string> allLines;
    while (std::getline(cin, line))
    {
        allLines.push_back(line);
    }
    const int numLines = static_cast<int>(allLines.size());

    vector<string> currentDirComponents = { "UNKNOWN" };

    map<vector<string>, set<DirEntry>> entriesForPath;

    for (int lineIndex = 0; lineIndex < numLines;)
    {
        const std::string& line = allLines[lineIndex];

        std::cout << "line: " << line << " current path: ";
        for (const auto& component : currentDirComponents)
        {
            cout << component << "/";
        }
        cout << endl;

        assert(!line.empty());
        istringstream lineStream(line);
        char firstChar;
        lineStream >> firstChar;
        assert(firstChar == '$');
        string command;
        lineStream >> command;

        if (command == "cd")
        {
            string targetDir;
            lineStream >> targetDir;
            std::cout << "cd to " << targetDir << std::endl;
            if (targetDir == "..")
            {
                assert(!currentDirComponents.empty());
                currentDirComponents.pop_back();
            }
            else if (targetDir == "/")
            {
                currentDirComponents.clear();
            }
            else
            {
                currentDirComponents.push_back(targetDir);
            }

            lineIndex++;
            continue;
        }
        else if (command == "ls")
        {
            lineIndex++;
            std::cout << "Running ls in path: ";
            for (const auto& component : currentDirComponents)
            {
                cout << component << "/";
            }
            cout << endl;
            while (lineIndex < numLines && allLines[lineIndex][0] != '$')
            {
                istringstream dirLineStream{allLines[lineIndex]};
                string firstWord;
                dirLineStream >> firstWord;
                if (firstWord == "dir")
                {
                    string dirName;
                    dirLineStream >> dirName;

                    DirEntry dir;
                    dir.isDir = true;
                    dir.size = 0;
                    dir.name = dirName;

                    entriesForPath[currentDirComponents].insert(dir);
                }
                else
                {
                    const int64_t fileSize = stoull(firstWord);
                    string fileName;
                    dirLineStream >> fileName;

                    DirEntry file;
                    file.isDir = false;
                    file.size = fileSize;
                    file.name = fileName;

                    entriesForPath[currentDirComponents].insert(file);
                }
                lineIndex++;
            }
            continue;
        }
        else 
        {
            assert(false);
        }
    }

    printDirTree(entriesForPath, vector<string>());
    vector<int64_t> dirSizes;
    calcDirSize(entriesForPath, vector<string>(), dirSizes);

    int64_t totalSizesUnder100000 = 0;
    for (const auto dirSize : dirSizes)
    {
        if (dirSize <= 100000)
        {
            totalSizesUnder100000 += dirSize;
        }
    }
    std::cout << "totalSizesUnder100000: " << totalSizesUnder100000 << std::endl;

}
