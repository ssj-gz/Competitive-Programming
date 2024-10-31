#include <iostream>
#include <regex>
#include <map>

#include <cassert>

using namespace std;

int main()
{
    struct Scanner
    {
        int pos = 0;
        int range = -1;
        int dir = 1;
    };
    std::regex scannerRegex(R"(^(\d+):\s*(\d+)$)");
    string scannerLine;
    map<int, Scanner> scannerAtDepth;
    int finalScannerDepth = -1;
    while (std::getline(cin, scannerLine))
    {
        std::smatch scannerMatch;
        const bool matchSuccessful = std::regex_match(scannerLine, scannerMatch, scannerRegex);
        assert(matchSuccessful);
        const int depth = std::stoi(scannerMatch[1]);
        Scanner scanner;
        scanner.range = std::stoi(scannerMatch[2]);
        assert(scanner.range > 0);
        assert(depth >= 0);
        scannerAtDepth[depth] = scanner;

        finalScannerDepth = std::max(finalScannerDepth, depth);
    }

    int depth = -1;
    int64_t totalSeverity = 0;
    while (depth <= finalScannerDepth)
    {
        depth++;
        std::cout << "depth: " << depth << std::endl;
        if (scannerAtDepth.contains(depth))
        {
            std::cout << " scanner here at pos: " << scannerAtDepth[depth].pos << std::endl;
            if (scannerAtDepth[depth].pos == 0)
            {
                totalSeverity += depth * scannerAtDepth[depth].range;
                std::cout << "Caught at depth: " << depth << " by scanner with range: " << scannerAtDepth[depth].range << std::endl;
            }
        }

        for (auto& [depth, scanner] : scannerAtDepth)
        {
            if (scanner.dir == 1)
            {
                scanner.pos++;
                if (scanner.pos == scanner.range - 1)
                {
                    scanner.dir = -1;
                }
            }
            else
            {
                scanner.pos--;
                if (scanner.pos == 0)
                {
                    scanner.dir = 1;
                }
            }
        }

    }
    std::cout << "totalSeverity: " << totalSeverity << std::endl;

    return 0;
}
