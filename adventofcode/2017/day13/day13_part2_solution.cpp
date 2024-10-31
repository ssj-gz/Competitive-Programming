#include <iostream>
#include <regex>
#include <map>

#include <cassert>

using namespace std;

int main()
{
    struct Scanner
    {
        int range = -1;
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
    auto scannerPosAtBeginningOfPicoSecond = [](const auto& scanner, int picoSecond)
    {
        const int period = 2 * (scanner.range - 1);
        picoSecond = picoSecond % period;
        if (picoSecond <= scanner.range - 1)
        {
            return picoSecond;
        }
        else
        {
            return scanner.range - (picoSecond - (scanner.range - 1)) - 1;
        }
    };

    int delay = 0;
    while (true)
    {
        int depth = -1;
        bool wasCaught = false;

        int time = delay;
        while (depth <= finalScannerDepth && !wasCaught)
        {
            depth++;
            if (scannerAtDepth.contains(depth))
            {
                const auto& scanner = scannerAtDepth[depth];
                const int scannerPos = scannerPosAtBeginningOfPicoSecond(scanner, time);
                if (scannerPos == 0)
                {
                    wasCaught = true;
                }
            }
            time++;
        }
        if (!wasCaught)
            break;
        delay++;
    }
    std::cout << "delay: " << delay << std::endl;

    return 0;
}
