#include <iostream>
#include <regex>

#include <cassert>

using namespace std;

int main()
{
    // To continue, please consult the code grid in the manual.  Enter the code at row 3010, column 3019.
    std::regex codeRegex(R"(^To continue, please consult the code grid in the manual\.  Enter the code at row (\d+), column (\d+)\.$)");
    string codeLine;
    std::getline(cin, codeLine);

    std::smatch codeMatch;
    const bool matchSuccessful = std::regex_match(codeLine, codeMatch, codeRegex);
    assert(matchSuccessful);
    const int codeRow = std::stoi(codeMatch.str(1));
    const int codeCol = std::stoi(codeMatch.str(2));

    int64_t currentCode = 20151125;
    int64_t currentRow = 1;
    int64_t currentCol = 1;
    while (true)
    {
        //std::cout << "code at row: " << currentRow << " col: " << currentCol << " is: " << currentCode << std::endl;
        if (currentRow == codeRow && currentCol == codeCol)
        {
            std::cout << "result: " << currentCode <<  std::endl;
            break;
        }
        currentCode = (currentCode * 252533) % 33554393;
        currentCol++;
        currentRow--;
        if (currentRow == 0)
        {
            currentRow = currentCol;
            currentCol = 1;
        }
    }

    return 0;
}
